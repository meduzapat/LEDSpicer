/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      LedWiz32.cpp
 * @since     Nov 7, 2018
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

#include "LedWiz32.hpp"

using namespace LEDSpicer::Devices::GroovyGameGear;

uint8_t LedWiz32::dumpFrame = 0;

void LedWiz32::afterClaimInterface() {
	LogDebug("Initializing " + getFullName() + " controllers ICs");
	// This will initialize the 4 controllers and set the pulse to 1.
	vector<uint8_t> data {64, 255, 255, 255, 255, 1, 0, 0};
	transferToConnection(data);
}

void LedWiz32::drawHardwarePinMap() {

	uint8_t half = LEDWIZ32_LEDS / 2;
	cout
		<< getFullName() << " Pins " << LEDWIZ32_LEDS << endl
		<< "Hardware pin map:" << endl << "L     R"  << endl;
	for (uint8_t r = 0; r < half; ++r) {
		uint8_t l = LEDWIZ32_LEDS - r - 1;
		setLed(r, r + 1);
		setLed(l, l + 1);
		cout <<
			std::left << std::setfill(' ') << std::setw(3) << static_cast<int>(*getLed(r)) << "   " <<
			std::left << std::setfill(' ') << std::setw(3) << static_cast<int>(*getLed(l)) << endl;
	}
	cout << endl;
}

void LedWiz32::transfer() const {
	/*
	 * This device transfers chunks of 8 bits from 0 to 63.
	 * 0 to 48 with modulation.
	 * 49 to 63 without.
	 */
	vector<uint8_t> load;
	for (auto l : LEDs) {
		load.push_back(48 * (l / 255.00));
		if (load.size() == 8) {
			transferToConnection(load);
			std::this_thread::sleep_for(std::chrono::microseconds(LEDWIZ_WAIT));
			load.clear();
		}
	}
}

uint16_t LedWiz32::getProduct() const {
	return LEDWIZ32_PRODUCT + boardId - 1;
}
