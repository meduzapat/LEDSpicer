/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      USB.cpp
 * @since     Jul 4, 2020
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2018 - 2026 Patricio A. Rossi (MeduZa)
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

#include "USB.hpp"

using namespace LEDSpicer::Utilities;

libusb_context* USB::usbSession = nullptr;

USB::USB(uint16_t wValue, uint8_t  interface, uint8_t  boardId, uint8_t  maxBoards) :
	wValue(wValue), interface(interface), boardId(boardId) {

	if (not Utility::verifyValue<uint8_t>(boardId, 1, maxBoards, false))
		throw Error("Board id should be a number between 1 and ") << maxBoards;

	if (not usbSession) {
		LogInfo("Opening USB session");
		if (libusb_init(&usbSession) != LIBUSB_SUCCESS) throw Error("Unable to open USB session");
	}
	libusb_set_option(usbSession, LIBUSB_OPTION_LOG_LEVEL, LIBUSB_LOG_LEVEL_ERROR);
}

void USB::connect() {

	LogInfo("Connecting to " + Utility::hex2str(getVendor()) + ":" + Utility::hex2str(getProduct()) + " Id: " + to_string(boardId));

	if (handle) return;

	libusb_device** list = nullptr;
	libusb_device*  device = nullptr;

	libusb_get_device_list(usbSession, &list);

	if (isNonBasedId()) {
		// Devices with no unique identifier need topology-based sorting for stable ordering.
		struct Candidate {
			libusb_device*       dev;
			uint8_t              bus;
			std::vector<uint8_t> ports;
		};
		std::vector<Candidate> candidates;

		for (size_t idx = 0; list[idx] != nullptr; ++idx) {
			libusb_device_descriptor desc;
			if (libusb_get_device_descriptor(list[idx], &desc) != 0) continue;
			if (desc.idVendor != getVendor() or desc.idProduct != getProduct()) continue;

			Candidate c;
			c.dev = list[idx];
			c.bus = libusb_get_bus_number(list[idx]);
			uint8_t portBuf[8];
			int n = libusb_get_port_numbers(list[idx], portBuf, sizeof(portBuf));
			if (n > 0) {
				c.ports.assign(portBuf, portBuf + n);
			}
			candidates.push_back(std::move(c));
		}

		std::sort(candidates.begin(), candidates.end(), [](const Candidate& a, const Candidate& b) {
			if (a.bus != b.bus) {
				return a.bus < b.bus;
			}
			return a.ports < b.ports;
		});

		if (boardId <= candidates.size()) device = candidates[boardId - 1].dev;
	}
	else {
		for (size_t idx = 0; list[idx] != nullptr; ++idx) {
			device = list[idx];
			libusb_device_descriptor desc;
			libusb_get_device_descriptor(device, &desc);

			if (desc.idVendor == getVendor() and desc.idProduct == getProduct()) {
				// For ID by product only check the vendor and product.
				if (isProductBasedId()) {
					break;
				}
				// For independent hardware check device ID.
				else if (desc.bcdDevice == boardId) {
					break;
				}
			}
			device = nullptr;
		}
	}

	int rc = LIBUSB_ERROR_NO_DEVICE;
	if (device) rc = libusb_open(device, &handle);

	libusb_free_device_list(list, 1);

	if (rc == LIBUSB_SUCCESS) {
		libusb_set_auto_detach_kernel_driver(handle, true);
		return;
	}

	throw Error("Failed to open device ")  <<
			Utility::hex2str(getVendor())  << ":" <<
			Utility::hex2str(getProduct()) << " id " << getId();
}

void USB::disconnect() {

	if (not handle) return;

	libusb_release_interface(handle, interface);
	LogDebug("Reseting interface: " + to_string(interface));
	auto r = libusb_reset_device(handle);
	if (r) {
		LogWarning(string(libusb_error_name(r)));
	}
	LogDebug("Closing interface: " + to_string(interface));
	libusb_close(handle);
	handle = nullptr;
}

void USB::claimInterface() {
	LogDebug("Claiming interface " + to_string(interface));
	if (libusb_claim_interface(handle, interface))
		throw Error("Unable to claim interface to ") <<
				Utility::hex2str(getVendor()) << ":" <<
				Utility::hex2str(getProduct());
}

void USB::closeSession() {

	if (not usbSession) return;

	LogInfo("Closing USB session");
	libusb_exit(usbSession);
	usbSession = nullptr;
}

uint8_t USB::getId() const {
	return boardId;
}

bool USB::isProductBasedId() const {
	return true;
}

bool USB::isNonBasedId() const {
	return false;
}

int USB::send(vector<uint8_t>& data) const {
	return libusb_control_transfer(
		handle,
		REQUEST_TYPE,
		REQUEST,
		wValue,
		interface,
		data.data(),
		data.size(),
		USB_TIMEOUT
	);
}

void USB::transferToConnection(vector<uint8_t>& data) const {
	Connection::transferToConnection(data);
	int responseCode;
	if ((responseCode = send(data)) >= 0) return;

	LogError(
		"Error sending to USB: wValue: "  + Utility::hex2str(wValue) +
		" wIndex: "  + Utility::hex2str(interface) +
		" wLength: " + to_string(data.size())
	);
	throw Error(libusb_error_name(responseCode));
}

vector<uint8_t> USB::transferFromConnection(uint size) const {
	std::vector<uint8_t> response(size);
	// TODO: Implement actual read via libusb if needed, or throw Error("Reads unsupported").
	return response;
}
