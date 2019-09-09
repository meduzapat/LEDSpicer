/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      ConnectionUSB.cpp
 * @since     Jun 19, 2018
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2018 - 2019 Patricio A. Rossi (MeduZa)
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

#include "ConnectionUSB.hpp"
#ifdef DRY_RUN
#include <iomanip>
#endif
using namespace LEDSpicer;

libusb_context* ConnectionUSB::usbSession = nullptr;
milliseconds ConnectionUSB::waitTime;

ConnectionUSB::ConnectionUSB(
		uint16_t wValue,
		uint8_t  interface,
		uint8_t  elements,
		uint8_t  maxBoards,
		uint8_t  boardId,
		const string& name
) :
	name(name)
{
#ifndef DRY_RUN
	if (not usbSession)
		throw Error("USB session not initialized");
#endif

	if (not Utility::verifyValue<uint8_t>(boardId, 1, maxBoards, false))
		throw Error("Board id should be a number between 1 and " + to_string(maxBoards));

	board.interface = interface;
	board.boardId   = boardId;
	board.value     = wValue;

	LEDs.resize(elements);
	LEDs.shrink_to_fit();
}

ConnectionUSB::~ConnectionUSB() {

	if (not handle)
		return;

	LogInfo("Releasing interface: " + to_string(board.interface));
	libusb_release_interface(handle, board.interface);
	auto r = libusb_reset_device(handle);
	if (r)
		LogWarning(getFullName() + " " + string(libusb_error_name(r)));
	libusb_close(handle);
	handle = nullptr;
}

void ConnectionUSB::initialize() {
	connect();
	afterConnect();
	claimInterface();
	afterClaimInterface();
}

void ConnectionUSB::connect() {

	LogInfo("Connecting to " + Utility::hex2str(getVendor()) + ":" + Utility::hex2str(getProduct()) + " " + getFullName());

	handle = libusb_open_device_with_vid_pid(usbSession, getVendor(), getProduct());

	if (not handle)
		throw Error(
			"Unable to connect to " +
			Utility::hex2str(getVendor()) + ":" + Utility::hex2str(getProduct()) +
			" " + getFullName()
		);

	libusb_set_auto_detach_kernel_driver(handle, true);
}

void ConnectionUSB::claimInterface() {

	LogInfo("Claiming interface " + to_string(board.interface));
	if (libusb_claim_interface(handle, board.interface))
		throw Error(
			"Unable to claim interface to " +
			to_string(getVendor()) + ":" + to_string(getProduct()) +
			" " + getFullName()
		);
}

void ConnectionUSB::openSession() {
#ifndef DRY_RUN

	if (usbSession)
		return;

	LogInfo("Opening USB session");

	if (libusb_init(&usbSession) != 0)
		throw new Error("Unable to open USB session");
#ifdef DEVELOP
/*
	LIBUSB_LOG_LEVEL_NONE
	LIBUSB_LOG_LEVEL_ERROR
	LIBUSB_LOG_LEVEL_WARNING
	LIBUSB_LOG_LEVEL_INFO
	LIBUSB_LOG_LEVEL_DEBUG
*/
	libusb_set_debug(usbSession, LIBUSB_LOG_LEVEL_INFO);
#endif
#else
	LogDebug("No USB - DRY RUN");
#endif
}

void ConnectionUSB::terminate() {
#ifndef DRY_RUN
	if (not usbSession)
		return;

	LogInfo("Closing USB session");
	libusb_exit(usbSession);
	usbSession = nullptr;
#endif
}

uint8_t ConnectionUSB::getNumberOfLeds() {
	return LEDs.size();
}

uint8_t ConnectionUSB::getId() {
	return board.boardId;
}

void ConnectionUSB::transferData(vector<uint8_t>& data) {

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
		board.value,
		board.interface,
		data.data(),
		data.size(),
		TIMEOUT
	);

	if (responseCode >= 0)
		return;

	LogDebug(
		"wValue: "  + Utility::hex2str(board.value) +
		" wIndex: "  + Utility::hex2str(board.interface) +
		" wLength: " + to_string(data.size())
	);
	throw Error(string(libusb_error_name(responseCode)) + " for " + getFullName());
#endif
}

void ConnectionUSB::setInterval(uint8_t waitTime) {
	ConnectionUSB::waitTime = milliseconds(waitTime);
	LogInfo("Set interval to " + to_string(ConnectionUSB::waitTime.count()) + "ms");
}

milliseconds ConnectionUSB::getInterval() {
	return waitTime;
}

string ConnectionUSB::getFullName() {
	return "Device: " + name + " Id: " + to_string(board.boardId);
}

void ConnectionUSB::wait(milliseconds wasted) {
	if (wasted < waitTime)
		std::this_thread::sleep_for(waitTime - wasted);
	else
		LogWarning("Frame took longer time to render (" + to_string(wasted.count()) + "ms) that the minimal wait time (" + to_string(waitTime.count()) + "ms), to fix this decrease the number of FPS in the configuration");
}
