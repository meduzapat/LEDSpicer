/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file		ConnectionUSB.hpp
 * @since		Jun 19, 2018
 * @author		Patricio A. Rossi (MeduZa)
 * @copyright	Copyright © 2018 Patricio A. Rossi (MeduZa)
 */

#ifndef CONNECTIONUSB_HPP_
#define CONNECTIONUSB_HPP_ 1

// For ints.
#include <cstdint>

// To handle usb devices.
#include <libusb.h>

// To handle dynamic arrays.
#include <vector>
using std::vector;

#include <chrono>
#include <thread>

#include "utility/Error.hpp"
#include "utility/Log.hpp"

/// The request type field for the setup packet.
#define UM_REQUEST_TYPE 0x21
/// The request field for the setup packet.
#define UM_REQUEST      9
/// Default USB timeout
#define TIMEOUT        100

namespace LEDSpicer {

/**
 * LEDSpicer::ConnectionUSB
 * @note All USBLib functions return 0 when success.
 * Every LED is a single element, RGB elements uses three LEDs.
 */
class ConnectionUSB {

public:

	ConnectionUSB(uint16_t requestType, uint16_t request);

	virtual ~ConnectionUSB();

	static void openSession(uint8_t debugLevel);

	void initialize();

	static void terminate();

	/**
	 * Transfers internal buffer data using 0x04
	 * @param code
	 */
	void transfer();

	/**
	 * Transfers miscellaneous data.
	 * @param data
	 */
	void transfer(vector<uint8_t> data);

	/**
	 * Returns a pointer to a single LED.
	 * @param ledPos
	 * @return
	 */
	uint8_t* getLED(uint8_t ledPos);

protected:

	/// USB device handle.
	libusb_device_handle* handle = nullptr;

	/// App wide libusb session.
	static libusb_context *usbSession;

	/// Internal structure to keep initialization information
	struct ConnectionData {
		const char* name = nullptr;
		uint16_t
			/// Vendor ID
			vendor  = 0,
			/// Product ID
			product = 0,
			/// The value field for the setup packet.
			value   = 0,
			/// 10 frames per second.
			fps     = 100;
		uint8_t
			/// Interface number
			interface = 0,
			/// Board ID
			boardId   = 0,
			/// Number of board LEDs
			LEDs      = 0;
	} board;

	vector<uint8_t> LEDs;

	uint8_t
		requestType = 0,
		request     = 0;

	virtual void afterConnect() = 0;

	void connect();

	void claimInterface();

	virtual void afterClaimInterface() = 0;

	void checkTransferError(int responseCode);
};

} /* namespace LEDSpicer */

#endif /* CONNECTIONUSB_HPP_ */
