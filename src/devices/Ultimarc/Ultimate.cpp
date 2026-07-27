/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      UltimarcUltimate.cpp
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

#include "Ultimate.hpp"

using namespace LEDSpicer::Devices::Ultimarc;

void Ultimate::resetLeds() {

	// Set Off Ramp Speed.
	vector<uint8_t> data ULTIMAGE_MSG(0xC0, 0);
	transferToConnection(data);

	// Turn off all LEDs and internal buffer.
	setLeds(0);
	data[ULTIMAGE_MSG_COMMAND] = 0x80;
	transferToConnection(data);
}

void Ultimate::afterConnect() {

	libusb_config_descriptor* configuration = nullptr;

	if (libusb_get_active_config_descriptor(libusb_get_device(handle), &configuration) != LIBUSB_SUCCESS)
		throw Error("Unable to read the configuration descriptor for ") << getFullName();

	/*
	 * The keyboard, the mouse and, when the board is in a game controller mode, the gamepad
	 * come first, so the LEDs are always on the last interface. How many precede it depends
	 * on the mode the board was switched to, not on the firmware version.
	 */
	const uint8_t
		count    = configuration->bNumInterfaces,
		position = count - 1;
	interface    = configuration->interface[position].altsetting[0].bInterfaceNumber;
	reportLength = reportDescriptorLength(configuration, position);

	libusb_free_config_descriptor(configuration);

	LogInfo(getFullName() + " uses interface " + to_string(interface) + " of " + to_string(count));
}

void Ultimate::afterClaimInterface() {

	// Claiming detached the kernel driver, so the report descriptor can be read now.
	hasFastLedReport = detectFastLedReport(reportLength);
	LogInfo(getFullName() + (hasFastLedReport ? " accepts single packet updates" : " uses paired updates"));
}

uint16_t Ultimate::reportDescriptorLength(const libusb_config_descriptor* configuration, uint8_t position) const {

	const libusb_interface_descriptor& descriptor = configuration->interface[position].altsetting[0];

	// The HID class descriptor trails the interface descriptor and sizes what is subordinate to it.
	for (int c = 0; c + 8 < descriptor.extra_length and descriptor.extra[c]; c += descriptor.extra[c])
		if (descriptor.extra[c + 1] == HID_DESCRIPTOR and descriptor.extra[c + 6] == HID_REPORT_DESCRIPTOR)
			return descriptor.extra[c + 7] | (descriptor.extra[c + 8] << 8);

	throw Error("No HID report descriptor on interface ") << to_string(interface) << " of " << getFullName();
}

bool Ultimate::detectFastLedReport(uint16_t length) const {

	vector<uint8_t> report(length);

	const int read = libusb_control_transfer(
		handle,
		LIBUSB_ENDPOINT_IN | LIBUSB_RECIPIENT_INTERFACE,
		LIBUSB_REQUEST_GET_DESCRIPTOR,
		HID_REPORT_DESCRIPTOR << 8,
		interface,
		report.data(),
		length,
		USB_TIMEOUT
	);

	if (read < 0)
		throw Error("Unable to read the report descriptor of ") << getFullName() << ": " << libusb_error_name(read);

	// Only the collection that takes every LED in one packet declares this usage page.
	for (int c = 0; c + 1 < read;) {
		const uint8_t size = report[c] & HID_ITEM_SIZE;
		if (size == 1 and (report[c] & HID_ITEM_TAG) == HID_ITEM_USAGE_PAGE and report[c + 1] == IPAC_ULTIMATE_FAST_PAGE)
			return true;
		// A size of 3 means four bytes of data, every other size means as many bytes as its value.
		c += 1 + (size == 3 ? 4 : size);
	}

	return false;
}

void Ultimate::drawHardwareLedMap() {
	uint16_t
		half    = IPAC_ULTIMATE_LEDS / 2,
		fillerL = 1,
		fillerR = half + 1,
		rLed    = half;
	cout << getFullName() << " LEDs " << IPAC_ULTIMATE_LEDS << endl;
	cout << "Hardware connection map:"    << endl <<
			"R  G  B     B  G  R"  << endl;
	for (uint16_t lLed = 0; lLed < half; ++lLed) {
		setLed(lLed, fillerL++);
		setLed(fillerR - 1, fillerR);
		fillerR++;
		cout << std::left << std::setfill(' ') << std::setw(3) << static_cast<uint16_t>(*getLed(lLed));
		if (not ((lLed + 1) % 3)) {
			cout << "   ";
			for (uint16_t c = 0; c < 3; ++c)
				cout << std::left << std::setfill(' ') << std::setw(3) << static_cast<uint16_t>(*getLed(rLed++));
			cout << endl;
		}
	}
	cout << endl;
}

void Ultimate::transfer() const {

	if (not hasFastLedReport) {
		transferPairs(ULTIMAGE_MSG(0, 0), ULTIMAGE_MSG_COMMAND);
		return;
	}

	vector<uint8_t> load;
	load.push_back(IPAC_ULTIMATE_FAST_REPORT);
	load.insert(load.end(), LEDs.begin(), LEDs.end());
	transferToConnection(load);
}

uint16_t Ultimate::getProduct() const {
	return IPAC_ULTIMATE_PRODUCT;
}

bool Ultimate::matchesSignature(const libusb_device_descriptor& descriptor) const {

	if (descriptor.idVendor != getVendor())
		return false;

	if (descriptor.idProduct < IPAC_ULTIMATE_PRODUCT or descriptor.idProduct > IPAC_ULTIMATE_PRODUCT_END)
		return false;

	// The low bit of the offset is the board, the high bit is the mode, which is the board's to pick.
	return ((descriptor.idProduct - IPAC_ULTIMATE_PRODUCT) & 0x01) == boardId - 1;
}

string Ultimate::notFoundHint() const {
	return ", the board may be in Xinput mode, where it has no LED interface";
}
