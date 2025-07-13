/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      USB.cpp
 * @since     Jul 4, 2020
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

#include "USB.hpp"

using namespace LEDSpicer;

libusb_context* USB::usbSession = nullptr;

USB::USB(uint16_t wValue, uint8_t  interface, uint8_t  boardId, uint8_t  maxBoards) :
	wValue(wValue), interface(interface), boardId(boardId) {

	if (not Utility::verifyValue<uint8_t>(boardId, 1, maxBoards, false))
		throw Error("Board id should be a number between 1 and " + to_string(maxBoards));

#ifdef DRY_RUN
	LogDebug("No USB Init - DRY RUN");
	return;
#endif

	if (not usbSession) {
		LogInfo("Opening USB session");
		if (libusb_init(&usbSession) != 0)
			throw new Error("Unable to open USB session");
	}
#ifdef SHOW_USB_OUTPUT
	/*
		LIBUSB_LOG_LEVEL_NONE
		LIBUSB_LOG_LEVEL_ERROR
		LIBUSB_LOG_LEVEL_WARNING
		LIBUSB_LOG_LEVEL_INFO
		LIBUSB_LOG_LEVEL_DEBUG
	*/
		#if LIBUSB_API_VERSION >= 0x01000106
		libusb_set_option(usbSession, LIBUSB_OPTION_LOG_LEVEL, LIBUSB_LOG_LEVEL_DEBUG);
		#else
		libusb_set_debug(usbSession, LIBUSB_LOG_LEVEL_DEBUG);
		#endif
#else
		#if LIBUSB_API_VERSION >= 0x01000106
		libusb_set_option(usbSession, LIBUSB_OPTION_LOG_LEVEL, LIBUSB_LOG_LEVEL_ERROR);
		#else
		libusb_set_debug(usbSession, LIBUSB_LOG_LEVEL_ERROR);
		#endif
#endif
}

void USB::connect() {

	LogInfo("Connecting to " + Utility::hex2str(getVendor()) + ":" + Utility::hex2str(getProduct()) + " Id: " + to_string(boardId));

#ifdef DRY_RUN
	LogDebug("No USB handle - DRY RUN");
	return;
#endif

	if (handle)
		return;

	libusb_device** list;
	libusb_device* device;
	libusb_get_device_list(usbSession, &list);
	for (size_t idx = 0; list[idx] != nullptr; idx++) {
		device = list[idx];
		libusb_device_descriptor desc = {0};
		libusb_get_device_descriptor(device, &desc);

		if (desc.idVendor == getVendor() and desc.idProduct == getProduct()) {
			// For ID by product only check the vendor and product.
			if (isProductBasedId()) {
				break;
			}
			// For hardware that have no order use the position on the list.
			else if (isNonBasedId() and idx + 1 == boardId) {
				break;
			}
			// For independent hardware check device ID.
			else if (desc.bcdDevice == boardId) {
				break;
			}
		}
		device = nullptr;
	}
	libusb_free_device_list(list, 1);

	if (device and libusb_open(device, &handle) == LIBUSB_SUCCESS) {
		libusb_set_auto_detach_kernel_driver(handle, true);
		return;
	}
	throw Error("Failed to open device " + Utility::hex2str(getVendor()) + ":" + Utility::hex2str(getProduct()) + " id " + to_string(getId()));
}

void USB::disconnect() {

#ifdef DRY_RUN
	LogDebug("No disconnect - DRY RUN");
#endif

	if (not handle)
		return;

	libusb_release_interface(handle, interface);
#ifdef DEVELOP
	LogDebug("Reseting interface: " + to_string(interface));
#endif
	auto r = libusb_reset_device(handle);
	if (r) {
		LogWarning(string(libusb_error_name(r)));
	}
#ifdef DEVELOP
	LogDebug("Closing interface: " + to_string(interface));
#endif
	libusb_close(handle);
	handle = nullptr;
}

void USB::claimInterface() {

#ifdef DRY_RUN
	LogDebug("No USB interface - DRY RUN");
	return;
#endif

	LogDebug("Claiming interface " + to_string(interface));
	if (libusb_claim_interface(handle, interface))
		throw Error(
			"Unable to claim interface to " +
			Utility::hex2str(getVendor()) + ":" + Utility::hex2str(getProduct())
		);
}

void USB::closeSession() {

#ifdef DRY_RUN
	LogDebug("No Close USB - DRY RUN");
	return;
#endif

	if (not usbSession)
		return;

	LogInfo("Closing USB session");
	libusb_exit(usbSession);
	usbSession = nullptr;
}

uint8_t USB::getId() const {
	return boardId;
}

const bool USB::isProductBasedId() const {
	return true;
}

const bool USB::isNonBasedId() const {
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

#ifdef SHOW_OUTPUT
	cout << "Data sent:" << endl;
	uint8_t count = MAX_DUMP_COLUMNS;
	for (auto b : data) {
		cout << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << static_cast<int>(b) << ' ';
		if (not --count) {
			count = MAX_DUMP_COLUMNS;
			cout << endl;
		}
	}
	cout << endl;
#endif

#ifndef DRY_RUN
	int responseCode;
	if ((responseCode = send(data)) >= 0)
		return;

	LogDebug(
		"Error sending to USB: wValue: "  + Utility::hex2str(wValue) +
		" wIndex: "  + Utility::hex2str(interface) +
		" wLength: " + to_string(data.size())
	);
	throw Error(string(libusb_error_name(responseCode)));
#endif
}

vector<uint8_t> USB::transferFromConnection(uint size) const {
	std::vector<uint8_t> response(size);
	return response;
}
