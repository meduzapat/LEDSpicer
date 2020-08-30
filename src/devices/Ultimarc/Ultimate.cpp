/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      UltimarcUltimate.cpp
 * @since     Jun 23, 2018
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

#include "Ultimate.hpp"

using namespace LEDSpicer::Devices::Ultimarc;

void Ultimate::resetLeds() {

	// Set Off Ramp Speed.
	vector<uint8_t> data ULTIMAGE_MSG(0xC0, 0);
	transferToUSB(data);

	// Turn off all LEDs and internal buffer.
	setLeds(0);
	data[3] = 0x80;
	transferToUSB(data);
}

void Ultimate::afterConnect() {

	// Detect interface.
	LogDebug("Detecting interface");

	libusb_device *device = libusb_get_device(handle);

	if (!device)
		throw Error("Unable to retrieve device information");

	libusb_device_descriptor descriptor;

	libusb_get_device_descriptor(device, &descriptor);

	// Detect Game Controller mode (will shift the interface by one).
	if ((descriptor.bcdDevice & 0x40)) {
		LogInfo("No Game Controller mode detected");
		interface = IPAC_ULTIMATE_INTERFACE;
	}
	else {
		LogInfo("Game Controller mode detected");
		interface = IPAC_ULTIMATE_INTERFACE + 1;
	}
}

void Ultimate::drawHardwarePinMap() {
	uint8_t
		half    = IPAC_ULTIMATE_LEDS / 2,
		fillerL = 1,
		fillerR = half + 1,
		rLed    = half;
	cout << getFullName() << " Pins " << IPAC_ULTIMATE_LEDS << endl;
	cout << "Hardware pin map:"    << endl <<
			"R  G  B     B  G  R"  << endl;
	for (uint8_t lLed = 0; lLed < half; ++lLed) {
		setLed(lLed, fillerL++);
		setLed(fillerR - 1, fillerR);
		fillerR++;
		cout << std::left << std::setfill(' ') << std::setw(3) << (int)*getLed(lLed);
		if (not ((lLed + 1) % 3)) {
			cout << "   ";
			for (uint8_t c = 0; c < 3; ++c)
				cout << std::left << std::setfill(' ') << std::setw(3) << (int)*getLed(rLed++);
			cout << endl;
		}
	}
	cout << endl;
}

void Ultimate::transfer() const {

	vector<uint8_t> load;
	load.push_back(0x04);
	load.insert(load.end(), LEDs.begin(), LEDs.end());
	transferToUSB(load);
}

uint16_t Ultimate::getProduct() const {
	return IPAC_ULTIMATE_PRODUCT + boardId - 1;
}
