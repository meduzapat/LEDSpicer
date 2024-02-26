/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      GPWiz40RotoX.cpp
 * @since     Aug 11, 2020
 * @author    Patricio A. Rossi (MeduZa) & GPWiz40RotoX related code Chris Newton (mahuti)
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

#include "GPWiz40RotoX.hpp"

using namespace LEDSpicer::Restrictors;

uint8_t GPWiz40RotoX::getMaxIds() const {
	return GPWIZ40ROTOX_MAX_ID;
}

void GPWiz40RotoX::rotate(const umap<string, Ways>& playersData) {

	/*
	bit0 (1) = 12-way rotary enable on Rotary Stick 1
	bit1 (2) = 12-way rotary enable on Rotary Stick 2
	bit4 (16) = 8-way rotary enable on Rotary Stick 1
	bit5 (32) = 8-way rotary enable on Rotary Stick 2
	 */
	vector<uint8_t> data {207, speedOn, speedOff, 0};

	for (auto& p : players)
		if (playersData.count(p.first) and isRotary(playersData.at(p.first))) {
			data[3] += p.second + (playersData.at(p.first) == Ways::rotary8 ? (p.second == 1 ? 16 : 32) : 0);
			LogDebug("Rotating " + getFullName() + " Rotor " +  to_string(p.second) + " to " + ways2str(playersData.at(p.first)));
		}

	if (data[3])
		transferToConnection(data);
}

uint16_t GPWiz40RotoX::getVendor() const {
	return GGG_VENDOR;
}

uint16_t GPWiz40RotoX::getProduct() const {
	return (GPWIZ40ROTOX_PRODUCT + getId() - 1);
}
