/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      HowlerController.hpp
 * @since     Feb 5, 2020
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

#include "WolfWareTech.hpp"

#pragma once

#define HOWLER_NAME       "Howler"
#define HOWLER_PRODUCT    0x6800
#define HOWLER_WVALUE     0x00CE
#define HOWLER_INTERFACE  0
#define HOWLER_LEDS       96
#define HOWLER_MAX_BOARDS 4
#define HOWLER_IN_EP      0x02
#define HOWLER_OUT_EP     0x81

#define HOWLER_CMD_SET_RGB_LED_BANK 0x09

// use bank with 1 3 5, use row with 0 1 2
#define howlerBankA(bankNumber, row) {HOWLER_WVALUE, HOWLER_CMD_SET_RGB_LED_BANK, bankNumber, LEDs[row + 0],  LEDs[row + 12], LEDs[row + 15], LEDs[row + 18], LEDs[row + 21], LEDs[row + 24], LEDs[row + 27], LEDs[row + 30], LEDs[row + 69], LEDs[row + 66], LEDs[row + 63], LEDs[row + 60], LEDs[row + 57], LEDs[row + 54], LEDs[row + 51], LEDs[row + 3], 0, 0, 0, 0, 0}
// use bank with 2 4 6, use row with 0 1 2
#define howlerBankB(bankNumber, row) {HOWLER_WVALUE, HOWLER_CMD_SET_RGB_LED_BANK, bankNumber, LEDs[row + 33], LEDs[row + 36], LEDs[row + 39], LEDs[row + 42], LEDs[row + 45], LEDs[row + 48], LEDs[row + 6],  LEDs[row + 90], LEDs[row + 93], LEDs[row + 9],  LEDs[row + 87], LEDs[row + 84], LEDs[row + 81], LEDs[row + 78], LEDs[row + 75], LEDs[row + 72], 0, 0, 0, 0, 0}


namespace LEDSpicer::Devices::WolfWareTech {

/**
 * LEDSpicer::Devices::WolfWareTech::Howler
 */
class Howler: public WolfWareTech {

public:

	Howler(StringUMap& options) :
	WolfWareTech(
		HOWLER_WVALUE,
		HOWLER_INTERFACE,
		HOWLER_LEDS,
		HOWLER_MAX_BOARDS,
		options,
		HOWLER_NAME
	) {}

	virtual ~Howler() = default;

	void drawHardwareLedMap() override;

	void transfer() const override;

	uint16_t getProduct() const override;

protected:

	int send(vector<uint8_t>& data) const override;

};

deviceFactory(Howler)

} // namespace
