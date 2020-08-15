/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      GPWiz40RotoX.hpp
 * @since     Aug 11, 2020
 * @author    Patricio A. Rossi (MeduZa) & GPWiz40RotoX related code Chris Newton (mahuti)
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

#include "../restrictors/GPWiz40RotoX.hpp"

using namespace LEDSpicer::Restrictors;

void GPWiz40RotoX::rotate(Ways way) {
    
    /*
    bit0 (1) = 12-way rotary enable on Rotary Stick 1
    bit1 (2) = 12-way rotary enable on Rotary Stick 2 
    bit4 (16) = 8-way rotary enable on Rotary Stick 1
    bit5 (32) = 8-way rotary enable on Rotary Stick 2 
    */ 
    /* 30,30 is default speedon / speedoff value, it would be nice to have some control over these 2 values  */ 
	// vector<uint8_t> data {207,30,30,0x000001};
    vector<uint8_t> data {207,30,30,1,0,0,0};
    
	switch (way) {
	case Ways::rotary8:
            data[3]= 1; 
            data[4]= 2; 
            data[5]= 16; 
            data[6]= 32; 
            LogDebug("Rotating " + getName() + " to 8 way rotary.");
            break;
	case Ways::rotary12:
            data[3] = 1; 
            data[4] = 2; 
            LogDebug("Rotating " + getName() + " to 12 way rotary.");
            break;
	default:
            data[3] = 1; 
            data[4] = 2; 
            LogDebug("Rotating " + getName() + " to 12 way rotary.");
	}
	transferToUSB(data);
}

uint16_t GPWiz40RotoX::getVendor() {
	return GGG_VENDOR;
}

uint16_t GPWiz40RotoX::getProduct() {
	return (GPWIZ40ROTOX_PRODUCT + getId() - 1);
}

string GPWiz40RotoX::getName() {
	return string(GPWIZ40ROTOX_NAME) + " " + to_string(getId());
}
