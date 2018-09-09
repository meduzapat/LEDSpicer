/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file		ConnectionUSB.cpp
 * @since		Jun 19, 2018
 * @author		Patricio A. Rossi (MeduZa)
 * @copyright	Copyright © 2018 Patricio A. Rossi (MeduZa)
 */

#include "ConnectionUSB.hpp"
#ifdef DRY_RUN
#include <iomanip>
#endif
using namespace LEDSpicer;

libusb_context* ConnectionUSB::usbSession = nullptr;
uint8_t         ConnectionUSB::waitTime   = 0;

ConnectionUSB::ConnectionUSB(uint16_t requestType, uint16_t request) :
	requestType(requestType),
	request(request)
{
#ifndef DRY_RUN
	if (not usbSession)
		throw Error("USB session not initialized");
#endif
}

void ConnectionUSB::initialize() {
	connect();
	afterConnect();
	claimInterface();
	afterClaimInterface();
}

void ConnectionUSB::connect() {

#ifdef DEVELOP
	Log::debug("Connecting to " + to_string(board.vendor) + ":" + to_string(board.product) + " " + board.name);
#endif

	handle = libusb_open_device_with_vid_pid(usbSession, board.vendor, board.product);

	if (not handle)
		throw Error("Unable to connect to " + to_string(board.vendor) + ":" + to_string(board.product) + " " + board.name);

	libusb_set_auto_detach_kernel_driver(handle, true);
}

void ConnectionUSB::claimInterface() {

	Log::debug("Claiming interface " + to_string(board.interface));
	if (libusb_claim_interface(handle, board.interface))
		throw Error("Unable to claim interface to " + to_string(board.vendor) + ":" + to_string(board.product) + " " + board.name);
}

ConnectionUSB::~ConnectionUSB() {

	if (not handle)
		return;

	Log::debug("Releasing interface " + to_string(board.interface));
	libusb_release_interface(handle, board.interface);
	libusb_close(handle);
	handle = nullptr;
}

void ConnectionUSB::openSession() {
#ifndef DRY_RUN

	if (usbSession)
		return;

	Log::debug("Opening USB session");

	if (libusb_init(&usbSession) != 0)
		throw new Error("Unable to open USB session");

/*
	LIBUSB_LOG_LEVEL_NONE = 0
	LIBUSB_LOG_LEVEL_ERROR
	LIBUSB_LOG_LEVEL_WARNING
	LIBUSB_LOG_LEVEL_INFO
	LIBUSB_LOG_LEVEL_DEBUG
*/
#ifdef DEVELOP
	libusb_set_debug(usbSession, LIBUSB_LOG_LEVEL_INFO);
#endif
#endif
}

void ConnectionUSB::terminate() {
#ifdef DRY_RUN
	if (not usbSession)
		return;

	Log::debug("Closing USB session");
	libusb_exit(usbSession);
	usbSession = nullptr;
#endif
}

void ConnectionUSB::transfer(vector<uint8_t> data) {
#ifdef DRY_RUN
	uint8_t count = 40;
	for (auto pin : data) {
		cout << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << (int)pin << ' ';
		if (!count--) {
			count = 40;
			cout << endl;
		}
	}
	cout << endl;
	std::this_thread::sleep_for(std::chrono::milliseconds(waitTime));
#else
	checkTransferError(libusb_control_transfer(
		handle,
		requestType,
		request,
		board.value,
		board.interface,
		data.data(),
		data.size(),
		TIMEOUT
	));
#endif
}

void ConnectionUSB::transfer() {
#ifdef DRY_RUN
	uint8_t count = 40;
	for (auto pin : LEDs) {
		cout << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << (int)pin << ' ';
		if (!count--) {
			count = 40;
			cout << endl;
		}
	}
	cout << endl;
	std::this_thread::sleep_for(std::chrono::milliseconds(waitTime));
#else
	checkTransferError(libusb_control_transfer(
		handle,
		requestType,
		request,
		board.value,
		board.interface,
		LEDs.data(),
		LEDs.size(),
		TIMEOUT
	));
#endif
}

void ConnectionUSB::setInterval(uint8_t waitTime) {
	Log::debug("Set interval to " + to_string(waitTime) + "ms");
	ConnectionUSB::waitTime = waitTime;
}

uint8_t ConnectionUSB::getInterval() {
	return waitTime;
}

void ConnectionUSB::checkTransferError(int responseCode) {

	std::this_thread::sleep_for(std::chrono::milliseconds(waitTime));

	if (responseCode >= 0)
		return;

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
}


