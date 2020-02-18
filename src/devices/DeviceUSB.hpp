/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DeviceUSB.hpp
 *
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

// To handle USB devices.
#include <libusb.h>

#include "Device.hpp"

#ifndef DEVICEUSB_HPP_
#define DEVICEUSB_HPP_ 1

/// The request type field for the setup packet.
#define REQUEST_TYPE 0x21
/// The request field for the setup packet.
#define REQUEST      0x09

/// Default USB timeout
#define TIMEOUT 1500

/// the number of columns to display when dumping pins.
#define MAX_DUMP_COLUMNS 40

namespace LEDSpicer {
namespace Devices {

/**
 * LEDSpicer::Devices::Device
 * Generic Device settings and functionality.
 */
class DeviceUSB : public Device {

public:

	DeviceUSB(
		uint16_t wValue,
		uint8_t  interface,
		uint8_t  elements,
		uint8_t  maxBoards,
		uint8_t  boardId,
		const string& name
	);

	virtual ~DeviceUSB();

	/**
	 * This function will be used to close the USB session, need to be called only once when ledspicer exit.
	 */
	static void closeUSB();

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
	virtual string getFullName();

protected:

	/// USB device handle.
	libusb_device_handle* handle = nullptr;

	/// The value field for the setup packet.
	uint16_t value = 0;

	uint8_t
		/// Interface number
		interface = 0,
		/// Board ID
		boardId   = 0;

	/// App wide libusb session.
	static libusb_context *usbSession;

	virtual void openDevice();

	/**
	 * Connect to the USB board.
	 */
	virtual void connect();

	/**
	 * This function will be called after connect.
	 */
	virtual void afterConnect() = 0;

	/**
	 * Detach from kernel and claim the interface.
	 */
	virtual void claimInterface();

	/**
	 * if anything needs to be initialized after claiming goes here.
	 */
	virtual void afterClaimInterface() = 0;

	/**
	 * Transfers miscellaneous data into the USB device.
	 * @param data
	 */
	virtual void transferToUSB(vector<uint8_t>& data);
};

}} /* namespace LEDSpicer */

#endif /* DEVICEUSB_HPP_ */
