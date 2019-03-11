/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      ConnectionUSB.hpp
 * @since     Jun 19, 2018
 * @author    Patricio A. Rossi (MeduZa)
 * @copyright Copyright © 2018 - 2019 Patricio A. Rossi (MeduZa)
 */

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
#include "utility/Utility.hpp"

#ifndef CONNECTIONUSB_HPP_
#define CONNECTIONUSB_HPP_ 1

/// The request type field for the setup packet.
#define REQUEST_TYPE 0x21
/// The request field for the setup packet.
#define REQUEST      0x09

/// Default USB timeout
#define TIMEOUT 1500

/// the number of columns to display when dumping pins.
#define MAX_DUMP_COLUMNS 40

namespace LEDSpicer {

/**
 * LEDSpicer::ConnectionUSB
 * @note All USBLib functions return 0 when success.
 * Every LED is a single element, RGB elements uses three LEDs.
 * LEDs always have 255 levels of intensity even when they are on/off only.
 */
class ConnectionUSB {

public:

	ConnectionUSB(
		uint16_t wValue,
		uint8_t  interface,
		uint8_t  elements,
		uint8_t  maxBoards,
		uint8_t  boardId,
		const string& name
	);

	virtual ~ConnectionUSB();

	static void openSession();

	void initialize();

	static void terminate();

	/**
	 * This method will be called every time a transfer need to be done.
	 */
	virtual void transfer() = 0;

	/**
	 * Returns the number of LEDs (pins) this board controls.
	 * @return
	 */
	uint8_t getNumberOfLeds();

	/**
	 * @return the vendor's code.
	 */
	virtual uint16_t getVendor() = 0;

	/**
	 * @return the vendor's code.
	 */
	virtual uint16_t getProduct() = 0;

	/**
	 * Returns the board Id.
	 * @return
	 */
	uint8_t getId();

	/**
	 * @return the device name with Id.
	 */
	string getFullName();

	/**
	 * Populates the pins with the correct pin number and
	 * displays the pin in a similar way they are found on the hardware.
	 */
	virtual void drawHardwarePinMap() = 0;

	/**
	 * Transfers miscellaneous data.
	 * @param data
	 */
	virtual void transferData(vector<uint8_t>& data);

	/**
	 * Sets the interval ms.
	 * @param waitTime
	 */
	static void setInterval(uint8_t waitTime);

	/**
	 * Returns the interval ms.
	 * @return
	 */
	static uint8_t getInterval();

	/**
	 * Waits for a defined amount of ms.
	 */
	static void wait();

protected:

	/// USB device handle.
	libusb_device_handle* handle = nullptr;

	/// The device name.
	string name;

	/// App wide libusb session.
	static libusb_context *usbSession;

	/// Internal structure to keep initialization information
	struct ConnectionData {
		uint16_t
			/// The value field for the setup packet.
			value = 0;
		uint8_t
			/// Interface number
			interface = 0,
			/// Board ID
			boardId   = 0;
	} board;

	vector<uint8_t> LEDs;

	static uint8_t waitTime;

	virtual void afterConnect() = 0;

	/**
	 * Connect to the USB board.
	 */
	virtual void connect();

	/**
	 * Detach from kernel and claim the interface.
	 */
	virtual void claimInterface();

	/**
	 * if anything needs to be initialized after claiming goes here.
	 */
	virtual void afterClaimInterface() = 0;
};

} /* namespace LEDSpicer */

#endif /* CONNECTIONUSB_HPP_ */
