/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      PacDrive.cpp
 * @since     Sep 19, 2018
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2018 - 2025 Patricio A. Rossi (MeduZa)
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

void PacDrive::drawHardwareLedMap() {
	uint16_t half = PAC_DRIVE_LEDS / 2;
	cout
		<< getFullName() << " LEDs " << PAC_DRIVE_LEDS << endl
		<< "Hardware connect map:" << endl << "L     R"  << endl;
	for (uint16_t r = 0; r < half; ++r) {
		uint16_t l = PAC_DRIVE_LEDS - r - 1;
		setLed(r, r + 1);
		setLed(l, l + 1);
		cout <<
			std::left << std::setfill(' ') << std::setw(3) << static_cast<int>(*getLed(r)) <<
			"   " <<
			std::left << std::setfill(' ') << std::setw(3) << static_cast<int>(*getLed(l)) << endl;
	}
	cout << endl;
}

void PacDrive::transfer() const {

	vector<uint8_t> load {0, 0, 0, 0};

	for (uint16_t led = 0; led < PAC_DRIVE_LEDS; ++led) {
		if (LEDs[led] > changePoint) {
			// two groups of 8 bits.
			if (led < 8)
				load[3] |= 1 << led;
			else
				load[2] |= 1 << (led - 8);
		}
	}
	transferToConnection(load);
}

uint16_t PacDrive::getProduct() const {
	return PAC_DRIVE_PRODUCT;
}

const bool PacDrive::isProductBasedId() const {
	return false;
}
