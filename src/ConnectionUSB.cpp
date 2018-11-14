/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      ConnectionUSB.cpp
 * @since     Jun 19, 2018
 * @author    Patricio A. Rossi (MeduZa)
 * @copyright Copyright © 2018 Patricio A. Rossi (MeduZa)
 */

#include "ConnectionUSB.hpp"
#ifdef DRY_RUN
#include <iomanip>
#endif
using namespace LEDSpicer;

libusb_context* ConnectionUSB::usbSession = nullptr;
uint8_t         ConnectionUSB::waitTime   = 0;

ConnectionUSB::ConnectionUSB(
		uint16_t requestType,
		uint16_t request,
		uint16_t wValue,
		uint8_t  interface,
		uint8_t  elements,
		uint8_t  maxBoards,
		uint8_t  boardId
) :
	requestType(requestType),
	request(request)
{
#ifndef DRY_RUN
	if (not usbSession)
		throw Error("USB session not initialized");
#endif

	if (not Utility::verifyValue(boardId, 1, maxBoards, false))
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

	LogInfo("Releasing interface " + to_string(board.interface) + " Id " + to_string(board.boardId));
	libusb_release_interface(handle, board.interface);
	libusb_reset_device(handle);
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

	LogInfo("Connecting to " + Utility::hex2str(getVendor()) + ":" + Utility::hex2str(getProduct()) + " " + getName());

	handle = libusb_open_device_with_vid_pid(usbSession, getVendor(), getProduct());

	if (not handle)
		throw Error("Unable to connect to " + Utility::hex2str(getVendor()) + ":" + Utility::hex2str(getProduct()) + " " + getName());

	libusb_set_auto_detach_kernel_driver(handle, true);
}

void ConnectionUSB::claimInterface() {

	LogInfo("Claiming interface " + to_string(board.interface));
	if (libusb_claim_interface(handle, board.interface))
		throw Error("Unable to claim interface to " + to_string(getVendor()) + ":" + to_string(getProduct()) + " " + getName());
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

	std::this_thread::sleep_for(std::chrono::milliseconds(waitTime));

#ifdef DRY_RUN
	#ifdef NO_OUTPUT
	return;
	#endif;
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
		requestType,
		request,
		board.value,
		board.interface,
		data.data(),
		data.size(),
		TIMEOUT
	);

	if (responseCode >= 0)
		return;

	LogDebug(
		"bmRequestType: " + Utility::hex2str(requestType) +
		" bRequest: "     + Utility::hex2str(request) +
		" wValue: "       + Utility::hex2str(board.value) +
		" wIndex: "       + Utility::hex2str(board.interface) +
		" wLength: "      + to_string(data.size())
	);
	switch (responseCode) {
	case LIBUSB_ERROR_TIMEOUT:
		throw Error("Transfer timed out");
	case LIBUSB_ERROR_PIPE:
		throw Error("control request was not supported by the device");
	case LIBUSB_ERROR_NO_DEVICE:
		throw Error("Device has been disconnected");
	case LIBUSB_ERROR_BUSY:
		throw Error("Busy");
	case LIBUSB_ERROR_INVALID_PARAM:
		throw Error ("transfer size is larger than the operating system and/or hardware can support");
	default:
		throw Error ("Libusb error " + to_string(responseCode));
	}
#endif
}

void ConnectionUSB::setInterval(uint8_t waitTime) {
	LogInfo("Set interval to " + to_string(waitTime) + "ms");
	ConnectionUSB::waitTime = waitTime;
}

uint8_t ConnectionUSB::getInterval() {
	return waitTime;
}


