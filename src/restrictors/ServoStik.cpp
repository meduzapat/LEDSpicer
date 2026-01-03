/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      ServoStick.cpp
 * @since     Jul 9, 2020
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2018 - 2025 Patricio A. Rossi (MeduZa)
 *
 * @copyright LEDSpicer is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * @copyright LEDSpicer is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * @copyright You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "ServoStik.hpp"

#include <sstream>
#include <vector>
#include <algorithm>


using namespace LEDSpicer::Restrictors;

void ServoStik::rotate(const umap<string, Ways>& playersData) {

	// ServoStik can move two restrictors to the same ways, pick the first and ignore the rest.
	Ways way(playersData.begin()->second);
	if (way == Ways::invalid)
		return;
	switch (way) {
		case Ways::w2:
		case Ways::w2v:
		case Ways::w4:
			way = Ways::w4;
			break;
		default:
			way = Ways::w8;
	}
	string pd(playersData.begin()->first);

	// Retrieve the previous state.
	Ways prevState(retrieveWays(pd));

	// Check if the current state is different from the previous state
	if (prevState == way) {
		LogDebug("Rotation not necessary for " + getFullName() + " already set to " + ways2str(prevState));
		return;
	}

	vector<uint8_t> data {0, 0xdd, 0, 0};
	if (way == Ways::w4) {
		LogDebug("Rotating " + getFullName() + " to 4 ways.");
		data[3] = 0;
	}
	else {
		LogDebug("Rotating " + getFullName() + " to 8 ways.");
		data[3] = 1;
	}
	transferToConnection(data);

	// Store the current state for every config.
	for (auto& pd: playersData) {
		Ways way(pd.second);
		switch (way) {
			case Ways::invalid:
				continue;
			case Ways::w2:
			case Ways::w2v:
			case Ways::w4:
				way = Ways::w4;
				break;
			default:
				way = Ways::w8;
		}
		storeWays(pd.first, way);
	}
}

uint16_t ServoStik::getVendor() const {
	return ULTIMARC_VENDOR;
}

uint16_t ServoStik::getProduct() const {
	return (SERVOSTIK_PRODUCT);
}

uint8_t ServoStik::getMaxIds() const {
	return 2;
}

const bool ServoStik::isNonBasedId() const {
	return true;
}

void ServoStik::storeWays(const string& profile, const Ways& ways) const {

	string configDir = Utility::getConfigDir();
	// Check if the directory exists, if not, create it
	struct stat st;
	if (stat(configDir.c_str(), &st) != 0) {
		if (mkdir(configDir.c_str(), 0770) != 0) {
			LogDebug("Error: Unable to create directory " + configDir);
			return;
		}
	}
	else if (!S_ISDIR(st.st_mode)) {
		LogDebug("Error: Config directory path exists but is not a directory.");
		return;
	}

	string filePath(configDir + SERVOSTIK_DATA(profile));
	std::ofstream outputFile(filePath);
	if (outputFile.is_open()) {
		LogDebug("File " + filePath + " stored " + ways2str(ways));
		outputFile << ways2StrWays(ways);
		outputFile.close();
		return;
	}
	LogDebug("Unable to save " + filePath);
}

const Restrictor::Ways ServoStik::retrieveWays(const string& profile) const {
	std::string filePath(Utility::getConfigDir() + SERVOSTIK_DATA(profile));
	std::ifstream inputFile(filePath);
	if (inputFile.is_open()) {
		LogDebug("Reading " + filePath);
		const string content((std::istreambuf_iterator<char>(inputFile)), std::istreambuf_iterator<char>());
		const Ways way(strWays2Ways(content));
		LogDebug("File " + filePath + " got " + ways2str(way));
		inputFile.close();
		return way;
	}
	LogDebug("Unable to read " + filePath);
	return Ways::invalid;
}

void ServoStik::connect() {
	/*
     * This override exists because ServoStik controller boards:
     *  - all share the same VID/PID (d209:1700)
     *  - do NOT expose a unique USB serial number
     *  - therefore cannot be uniquely identified by libusb_open_device_with_vid_pid()
     *
     * We must enumerate all matching devices and choose the correct one
     * deterministically using either:
     *   1) an explicit usbPath from the config (preferred)
     *   2) boardId as "Nth matching ServoStik device" (fallback)
     */
    LogInfo("Connecting to " + Utility::hex2str(getVendor()) + ":" + Utility::hex2str(getProduct()) +
            " Id: " + std::to_string(boardId) +
            (usbPath ? (" usbPath: " + *usbPath) : ""));

#ifdef DRY_RUN
    LogDebug("No USB handle - DRY RUN");
    return;
#endif

    if (handle) return;


    libusb_device** list = nullptr;
    libusb_get_device_list(usbSession, &list);


	/*
     * We build a list of *candidate* ServoStik devices.
     * Each candidate stores:
     *  - the libusb_device*
     *  - its USB bus number
     *  - its USB port chain (topology)
     *  - a stable string form like "1-4.1"
     */
    struct Candidate {
        libusb_device* dev;
        uint8_t bus;
        std::vector<uint8_t> ports;
        std::string path;
    };

    std::vector<Candidate> cands;

    for (size_t i = 0; list[i] != nullptr; i++) {
        libusb_device* dev = list[i];
        libusb_device_descriptor desc{};
        if (libusb_get_device_descriptor(dev, &desc) != 0) continue;

        if (desc.idVendor != getVendor() || desc.idProduct != getProduct()) continue;

		/*
         * Retrieve USB topology info:
         *  - bus number (stable unless device moves controllers)
         *  - port chain (physical port path, e.g. 4 -> 1 = "4.1")
         */
        uint8_t bus = libusb_get_bus_number(dev);
        uint8_t ports[8];
        int n = libusb_get_port_numbers(dev, ports, sizeof(ports));

		/*
         * Build a stable, human-readable usbPath string:
         *   bus-port[.subport...]
         * Examples:
         *   1-3
         *   1-4.1
         */
        std::ostringstream oss;
        oss << (int)bus << "-";
        for (int p = 0; p < n; p++) {
            if (p) oss << ".";
            oss << (int)ports[p];
        }

        Candidate c;
        c.dev = dev;
        c.bus = bus;
        c.ports.assign(ports, ports + std::max(0, n));
        c.path = oss.str();
        cands.push_back(std::move(c));
    }

	/*
     * Sort candidates deterministically
     *
     * This ensures boardId fallback is stable:
     *   boardId=1 -> first physical ServoStik
     *   boardId=2 -> second physical ServoStik
     *
     * Sorting by:
     *   1) USB bus number
     *   2) USB port chain
     */
    std::sort(cands.begin(), cands.end(), [](const Candidate& a, const Candidate& b) {
        if (a.bus != b.bus) return a.bus < b.bus;
        return a.ports < b.ports;
    });

    libusb_device* chosen = nullptr;

	/*
     * If usbPath was specified in the config, try to match it exactly
     *
     * This is the most explicit and reliable method and avoids any ambiguity.
     */
    if (usbPath) {
        for (const auto& c : cands) {
            if (c.path == *usbPath) {
                chosen = c.dev;
                break;
            }
        }

		/*
         * If usbPath was provided but no device matched,
         * fall back to boardId so existing setups still work.
         */
        if (!chosen) {
            LogWarning("ServoStik usbPath '" + *usbPath + "' not found; falling back to boardId " + std::to_string(boardId));
        }
    }

	/*
     * Fallback selection using boardId
     *
     * boardId is 1-based:
     *   boardId=1 -> candidates[0]
     *   boardId=2 -> candidates[1]
     */
    if (!chosen) {
        if (boardId < 1 || boardId > cands.size()) {
            libusb_free_device_list(list, 1);
            throw Error("ServoStik boardId " + std::to_string(boardId) + " out of range; found " + std::to_string(cands.size()) + " device(s)");
        }
        chosen = cands[boardId - 1].dev;
    }

    int rc = libusb_open(chosen, &handle);
    libusb_free_device_list(list, 1);

    if (rc == LIBUSB_SUCCESS) {
        libusb_set_auto_detach_kernel_driver(handle, true);
        return;
    }

    throw Error("Failed to open ServoStik device id " + std::to_string(getId()));
}

