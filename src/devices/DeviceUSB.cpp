/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DeviceUSB.cpp
 * @since     Feb 11, 2020
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2018 - 2020 Patricio A. Rossi (MeduZa)
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

#include "DeviceUSB.hpp"
#ifdef DRY_RUN
#include <iomanip>
#endif
using namespace LEDSpicer::Devices;

libusb_context* DeviceUSB::usbSession = nullptr;

DeviceUSB::DeviceUSB(
		uint16_t wValue,
		uint8_t  interface,
		uint8_t  elements,
		uint8_t  maxBoards,
		uint8_t  boardId,
		const string& name
) :
	Device(elements, name),
	interface(interface),
	boardId(boardId),
	value(wValue)
{

	if (not Utility::verifyValue<uint8_t>(boardId, 1, maxBoards, false))
		throw Error("Board id should be a number between 1 and " + to_string(maxBoards));

#ifndef DRY_RUN
	if (not usbSession) {
		LogInfo("Opening USB session");
		if (libusb_init(&usbSession) != 0)
			throw new Error("Unable to open USB session");
	}
	#ifdef DEVELOP
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
#else
	LogDebug("No USB - DRY RUN");
#endif
}

DeviceUSB::~DeviceUSB() {

	if (not handle)
		return;

	LogInfo("Releasing interface: " + to_string(interface));
	libusb_release_interface(handle, interface);
	auto r = libusb_reset_device(handle);
	if (r)
		LogWarning(getFullName() + " " + string(libusb_error_name(r)));
	libusb_close(handle);
	handle = nullptr;
}

void DeviceUSB::openDevice() {
	connect();
	afterConnect();
	claimInterface();
	afterClaimInterface();
}

void DeviceUSB::connect() {

	LogInfo("Connecting to " + Utility::hex2str(getVendor()) + ":" + Utility::hex2str(getProduct()) + " " + getFullName());

	if (not (handle = libusb_open_device_with_vid_pid(usbSession, getVendor(), getProduct())))
		throw Error(
			"Unable to connect to " +
			Utility::hex2str(getVendor()) + ":" + Utility::hex2str(getProduct()) +
			" " + getFullName()
		);

	libusb_set_auto_detach_kernel_driver(handle, true);
}

void DeviceUSB::claimInterface() {

	LogInfo("Claiming interface " + to_string(interface));
	if (libusb_claim_interface(handle, interface))
		throw Error(
			"Unable to claim interface to " +
			to_string(getVendor()) + ":" + to_string(getProduct()) +
			" " + getFullName()
		);
}

void DeviceUSB::closeUSB() {
#ifndef DRY_RUN
	if (not usbSession)
		return;

	LogInfo("Closing USB session");
	libusb_exit(usbSession);
	usbSession = nullptr;
#endif
}

uint8_t DeviceUSB::getId() {
	return boardId;
}

string DeviceUSB::getFullName() {
	return "Device: " + name + " Id: " + to_string(boardId);
}

void DeviceUSB::transferToUSB(vector<uint8_t>& data) {

#ifdef DRY_RUN
	#ifdef NO_OUTPUT
	return;
	#endif
	uint8_t count = MAX_DUMP_COLUMNS;
	for (auto pin : data) {
		cout << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << (int)pin << ' ';
		if (!--count) {
			count = MAX_DUMP_COLUMNS;
			cout << endl;
		}
	}
	cout << endl;
#else

	auto responseCode = libusb_control_transfer(
		handle,
		REQUEST_TYPE,
		REQUEST,
		value,
		interface,
		data.data(),
		data.size(),
		TIMEOUT
	);

	if (responseCode >= 0)
		return;

	LogDebug(
		"wValue: "  + Utility::hex2str(value) +
		" wIndex: "  + Utility::hex2str(interface) +
		" wLength: " + to_string(data.size())
	);
	throw Error(string(libusb_error_name(responseCode)) + " for " + getFullName());
#endif
}

