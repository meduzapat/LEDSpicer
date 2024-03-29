/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      ServoStick.cpp
 * @since     Jul 9, 2020
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2018 - 2024 Patricio A. Rossi (MeduZa)
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

#include "ServoStik.hpp"

using namespace LEDSpicer::Restrictors;

void ServoStik::rotate(const umap<string, Ways>& playersData) {

	// ServoStik can move two restrictors to the same ways, pick the first and ignore the rest.
	Ways way = playersData.begin()->second;
	if (way == Ways::invalid)
		return;

	vector<uint8_t> data {0, 0xdd, 0, 0};
	switch (way) {
	case Ways::w2:
	case Ways::w2v:
	case Ways::w4:
		LogDebug("Rotating " + getFullName() + " to 4 ways.");
		data[3] = 0;
		break;
	default:
		LogDebug("Rotating " + getFullName() + " to 8 ways.");
		data[3] = 1;
	}
	transferToConnection(data);
}

uint16_t ServoStik::getVendor() const {
	return ULTIMARC_VENDOR;
}

uint16_t ServoStik::getProduct() const {
	return (SERVOSTIK_PRODUCT + getId() - 1);
}

uint8_t ServoStik::getMaxIds() const {
	return 2;
}
