/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Ultimate.hpp
 * @since     Jun 23, 2018
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

#include "Ultimarc.hpp"

#pragma once

#define IPAC_ULTIMATE_NAME       "Ultimarc Ipac Ultimate IO"
#define IPAC_ULTIMATE_WVALUE     0x0203
#define IPAC_ULTIMATE_LEDS       96
#define IPAC_ULTIMAGE_MAX_BOARDS 2
#define IPAC_ULTIMAGE_TRANSFER   0// All.

/// First and last product code of the block the board positions and the operating modes share.
#define IPAC_ULTIMATE_PRODUCT     0x0410
#define IPAC_ULTIMATE_PRODUCT_END 0x0413

/// Report ID of the single packet that carries every LED at once.
#define IPAC_ULTIMATE_FAST_REPORT 0x04
/// HID usage page of the collection that publishes the single packet report.
#define IPAC_ULTIMATE_FAST_PAGE   6

/// Descriptor type of the HID class descriptor.
#define HID_DESCRIPTOR        0x21
/// Descriptor type of the HID report descriptor.
#define HID_REPORT_DESCRIPTOR 0x22
/// Mask for the data size carried by a HID short item.
#define HID_ITEM_SIZE         0x03
/// Mask for the type and the tag of a HID short item.
#define HID_ITEM_TAG          0xFC
/// The HID short item tag that declares a usage page.
#define HID_ITEM_USAGE_PAGE   0x04

#define ULTIMAGE_MSG(byte1, byte2) {0x03, byte1, byte2, 0, 0}
/// Index of the command byte inside ULTIMAGE_MSG.
#define ULTIMAGE_MSG_COMMAND 1

namespace LEDSpicer::Devices::Ultimarc {

/**
 * LEDSpicer::Devices::Ultimate
 * Class to keep Ultimate I/O data and functionality.
 */
class Ultimate : public Ultimarc {

public:

	/**
	 * @param options
	 */
	Ultimate(StringUMap& options) :
	Ultimarc(
		IPAC_ULTIMATE_WVALUE,
		0, // to be defined.
		IPAC_ULTIMATE_LEDS,
		IPAC_ULTIMAGE_MAX_BOARDS,
		options,
		IPAC_ULTIMATE_NAME
	) {}

	virtual ~Ultimate() = default;

	void drawHardwareLedMap() override;

	void transfer() const override;

	uint16_t getProduct() const override;

	/**
	 * The product code block is shared by the board position and the operating mode, so the
	 * board is read back from it instead of being used to compute it.
	 */
	bool matchesSignature(const libusb_device_descriptor& descriptor) const override;

	void resetLeds() override;

protected:

	/**
	 * True when the board publishes the collection that accepts every LED in one packet.
	 * Stays false on DRY_RUN builds, where the probe never runs.
	 */
	bool hasFastLedReport = false;

	/// Length of the LED interface report descriptor, read while connecting.
	uint16_t reportLength = 0;

	void afterConnect() override;

	void afterClaimInterface() override;

	string notFoundHint() const override;

	/**
	 * Reads how long the HID report descriptor of an interface is.
	 *
	 * @param configuration the active configuration descriptor.
	 * @param position index of the interface inside the configuration.
	 * @return the length.
	 * @throws Error if the interface publishes no HID report descriptor.
	 */
	uint16_t reportDescriptorLength(const libusb_config_descriptor* configuration, uint8_t position) const;

	/**
	 * Reads the report descriptor of the claimed interface and looks for the collection
	 * that publishes the single packet report.
	 *
	 * @param length the report descriptor length.
	 * @return true when the board accepts the single packet report.
	 */
	bool detectFastLedReport(uint16_t length) const;

};

deviceFactory(Ultimate)

} // namespace
