/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      GPWiz49.cpp
 * @since     Aug 11, 2020
 * @author    Patricio A. Rossi (MeduZa) & GPWiz49 related code Chris Newton (mahuti)
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

#include "GPWiz49.hpp"

using namespace LEDSpicer::Restrictors;

void GPWiz49::rotate(Ways way) {
	vector<uint8_t> data {204, 1};
    /*
    1: 49-way (default)
    2: Progressive 49
    3: 8-Way
    4: 4-Way
    5: 4-Way Diagonal
    6: 2-Way Horizontal
    7: 2-Way Vertical
    8: 16-Way / Large dead zone in center
    */ 
    
	switch (way) {
	case Ways::w2:
            LogDebug("Rotating " + getName() + " to 2 way horizontal.");
           data[1]= 6;
            break;
	case Ways::w2v:
            LogDebug("Rotating " + getName() + " to 2 way vertical.");
            data[1]= 7;
            break;
	case Ways::w4:
            LogDebug("Rotating " + getName() + " to 4 ways.");
            data[1]= 4;
            break;
	case Ways::w4x:
            LogDebug("Rotating " + getName() + " to 4 ways diagonal.");
            data[1]= 5; 
            break;
    case Ways::w8:
            LogDebug("Rotating " + getName() + " to 8 ways.");
            data[1]= 3; 
            break;
    case Ways::w16:
            LogDebug("Rotating " + getName() + " to 16 ways with large dead zone in center.");
            data[1]= 8; 
            break;
    case Ways::w49:
            LogDebug("Rotating " + getName() + " to 49 ways - progressive");
            data[1]= 2; 
            break;
     case Ways::analog:
     case Ways::mouse:
            LogDebug("Rotating " + getName() + " 49 ways Raw");
            data[1]= 1; 
            break;                   
	default:
		LogDebug("Rotating " + getName() + " to 8 ways.");
		data[1] = 3;
	}
	transferToUSB(data);
  
}

uint16_t GPWiz49::getVendor() {
	return GGG_VENDOR;
}

uint16_t GPWiz49::getProduct() {
	return (GPWIZ49_PRODUCT + getId() - 1);
}

string GPWiz49::getName() {
	return string(GPWIZ49_NAME) + " " + to_string(getId());
}
