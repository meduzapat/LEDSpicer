/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      HowlerController.cpp
 * @since     Feb 5, 2020
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2020 Patricio A. Rossi (MeduZa)
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

#include "Howler.hpp"

using namespace LEDSpicer::Devices::WolfWareTech;

void Howler::resetLeds() {
//#define CMD_SET_GLOBAL_BRIGHTNESS	0x06
}

void Howler::drawHardwarePinMap() {
	cout
		<< getFullName() << " Pins " << HOWLER_LEDS << endl
		<< "Hardware pin map:" << endl;
	cout << endl;
}

void Howler::transfer() {
//#define CMD_SET_RGB_LED				0x01
//#define CMD_SET_INDIVIDUAL_LED		0x02
	vector<uint8_t> data;
	transferToUSB(data);
}

uint16_t Howler::getProduct() {
	return HOWLER_PRODUCT;
}

