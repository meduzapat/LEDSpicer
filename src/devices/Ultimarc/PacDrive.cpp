/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      PacDrive.cpp
 * @since     Sep 19, 2018
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

#include "PacDrive.hpp"

using namespace LEDSpicer::Devices::Ultimarc;

void PacDrive::resetLeds() {
	setLeds(0);
	transfer();
}

PacDrive::PacDrive(uint8_t boardId, umap<string, string>& options) :
	Ultimarc(
		PAC_DRIVE_WVALUE,
		PAC_DRIVE_INTERFACE,
		PAC_DRIVE_LEDS,
		PAC_DRIVE_MAX_BOARDS,
		boardId,
		PAC_DRIVE_NAME
	) {

	if (options.count("changePoint"))
		changePoint = Utility::parseNumber(
			options["changePoint"],
			"Invalid value for changePoint " + getFullName()
		);
}

void PacDrive::drawHardwarePinMap() {
	uint8_t half = PAC_DRIVE_LEDS / 2;
	cout
		<< getFullName() << " Pins " << PAC_DRIVE_LEDS << endl
		<< "Hardware pin map:" << endl << "L     R"  << endl;
	for (uint8_t r = 0; r < half; ++r) {
		uint8_t l = PAC_DRIVE_LEDS - r - 1;
		setLed(r, r + 1);
		setLed(l, l + 1);
		cout <<
			std::left << std::setfill(' ') << std::setw(3) << (int)*getLed(r) <<
			"   " <<
			std::left << std::setfill(' ') << std::setw(3) << (int)*getLed(l) << endl;
	}
	cout << endl;
}

void PacDrive::transfer() {

	vector<uint8_t> load {0, 0, 0, 0};

	for (uint8_t led = 0; led < PAC_DRIVE_LEDS; ++led) {
		if (LEDs[led] > changePoint) {
			// two groups of 8 bits.
			if (led < 8)
				load[3] |= 1 << led;
			else
				load[2] |= 1 << (led - 8);
		}
	}
	transferToUSB(load);
}

uint16_t PacDrive::getProduct() {
	return PAC_DRIVE_PRODUCT;
}

void PacDrive::connect() {

	LogInfo("Connecting to " + Utility::hex2str(getVendor()) + ":" + Utility::hex2str(getProduct()) + " " + getFullName());

	libusb_device** list;
	libusb_device* device;
	libusb_get_device_list(usbSession, &list);
	for (size_t idx = 0; list[idx] != nullptr; idx++) {
		device = list[idx];
		libusb_device_descriptor desc = {0};
		libusb_get_device_descriptor(device, &desc);
#ifdef DEVELOP
		LogDebug("Found: Vendor-Device-bcdDevice = " + Utility::hex2str(desc.idVendor) + "-" + Utility::hex2str(desc.idProduct) + "-" + to_string(desc.bcdDevice));
#endif
		if (desc.idVendor == getVendor() and desc.idProduct == getProduct() and desc.bcdDevice == boardId)
			break;
		device = nullptr;
	}
	libusb_free_device_list(list, 1);

	if (not device or libusb_open(device, &handle))
		throw Error(
			"Unable to connect to " +
			Utility::hex2str(getVendor()) + ":" + Utility::hex2str(getProduct()) +
			" " + getFullName()
		);
	libusb_set_auto_detach_kernel_driver(handle, true);
}
