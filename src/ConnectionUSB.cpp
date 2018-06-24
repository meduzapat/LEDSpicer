/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file		ConnectionUSB.cpp
 * @since		Jun 19, 2018
 * @author		Patricio A. Rossi (MeduZa)
 * @copyright	Copyright © 2018 Patricio A. Rossi (MeduZa)
 */

#include "ConnectionUSB.hpp"

using namespace LEDSpicer;

libusb_context* ConnectionUSB::usbSession = nullptr;

ConnectionUSB::ConnectionUSB(uint16_t requestType, uint16_t request) :
	requestType(requestType),
	request(request)
{

	if (not usbSession)
		throw Error("USB session not initialized");
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

void ConnectionUSB::openSession(uint8_t debugLevel) {

	if (usbSession)
		return;

	Log::debug("Opening USB session");

	if (libusb_init(&usbSession) != 0)
		throw new Error("Unable to open USB session");

	libusb_set_debug(usbSession, debugLevel);
}

void ConnectionUSB::terminate() {

	if (not usbSession)
		return;

	Log::debug("Closing USB session");
	libusb_exit(usbSession);
	usbSession = nullptr;
}

void ConnectionUSB::transfer(vector<uint8_t> data) {

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
}

void ConnectionUSB::transfer() {

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
}

uint8_t* ConnectionUSB::getLED(uint8_t ledPos) {
	return &LEDs[ledPos];
}

void ConnectionUSB::checkTransferError(int responseCode) {

	std::this_thread::sleep_for(std::chrono::milliseconds(board.fps));

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


