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
