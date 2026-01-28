/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      USB.hpp
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

#ifdef DRY_RUN
// To simulate handling USB devices.
#include "FakeLibUSB.hpp"
#else
// To handle USB devices.
#include <libusb.h>
#endif
#include "Connection.hpp"
#include "Brands.hpp"

#pragma once

/// The request type field for the setup packet.
#define REQUEST_TYPE LIBUSB_ENDPOINT_OUT | LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_RECIPIENT_INTERFACE
/// The request field for the setup packet.
#define REQUEST      0x09

/// Default USB timeout
#define USB_TIMEOUT 500

namespace LEDSpicer::Utilities {

/**
 * LEDSpicer::USB
 *
 * Class to handle connections to the USB controller.
 */
class USB : public Connection {

public:

	USB() = delete;

	/**
	 * Creates a new USB connection handler.
	 *
	 * @param wValue The setup packet value.
	 * @param interface The interface number.
	 * @param boardId The board identifier (1 to maxBoards).
	 * @param maxBoards Maximum allowed boards.
	 * @throws Error if boardId invalid.
	 */
	USB(uint16_t wValue, uint8_t  interface, uint8_t  boardId, uint8_t  maxBoards);

	virtual ~USB() = default;

	/**
	 * @return the vendor code.
	 */
	virtual uint16_t getVendor() const = 0;

	/**
	 * @return the vendor product code.
	 */
	virtual uint16_t getProduct() const = 0;

	/**
	 * Returns the board Id.
	 * @return
	 */
	uint8_t getId() const;

	/**
	 * Returns true if the Board ID modifies the USB product code, false if not.
	 * @return
	 */
	virtual bool isProductBasedId() const;

	/**
	 * Some hardware have no way to differentiate between the same product, so the ID will be the position on the USB list.
	 * @return
	 */
	virtual bool isNonBasedId() const;

	/**
	 * This function will be used to close the USB session,
	 * need to be called only once when ledspicer exit.
	 */
	static void closeSession();

protected:

	/// USB device handle.
	libusb_device_handle* handle = nullptr;

	/// The value field for the setup packet.
	uint16_t wValue = 0;

	uint8_t
		interface = 0, /// Interface number
		boardId   = 0; /// Board ID (1 = 1st)

	/// App wide libusb session.
	static libusb_context *usbSession;

	/**
	 * Connects to the USB board.
	 */
	virtual void connect();

	/**
	 * Claims the interface.
	 */
	void claimInterface();

	/**
	 * Disconnects from the USB board.
	 */
	virtual void disconnect();

	/**
	 * Prepares the data and send it to the connection function.
	 *
	 * @param data
	 */
	virtual void transferToConnection(vector<uint8_t>& data) const;

	/**
	 * Reads data from the USB connection (stub; override if device supports reads).
	 *
	 * @param size Amount of bytes to read.
	 * @return Vector of read data (zeros if unimplemented).
	 */
	virtual vector<uint8_t> transferFromConnection(uint size) const;

	/**
	 * This function will be called after connect.
	 */
	virtual void afterConnect() = 0;

	/**
	 * if anything needs to be initialized after claiming goes here.
	 */
	virtual void afterClaimInterface() = 0;

	/**
	 * Sends the data to the USB device.
	 *
	 * This function can be overridden to send data in a different way.
	 * Some devices need a different transmission method.
	 *
	 * @param data
	 * @return result code.
	 */
	virtual int send(vector<uint8_t>& data) const;

};

} // namespace
