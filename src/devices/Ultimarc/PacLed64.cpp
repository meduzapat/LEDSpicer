/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      PacLed64.cpp
 * @since     Feb 2, 2019
 * @author    Patricio A. Rossi (MeduZa)
 * @copyright Copyright © 2018 - 2024 Patricio A. Rossi (MeduZa)
 */

#include "PacLed64.hpp"

using namespace LEDSpicer::Devices::Ultimarc;

uint16_t PacLed64::getProduct() const {
	return PAC_LED64_PRODUCT + boardId - 1;
}

void PacLed64::drawHardwareLedMap() {

	cout << getFullName() << " LEDs " << PAC_LED64_LEDS << endl << "Hardware connection map:" << endl;
	for (uint16_t led = 0; led < PAC_LED64_LEDS; ++led)
		setLed(led, led + 1);
	for (uint16_t led = 0; led < 15; ++led) {
		if (not (led % 3))
			cout << "+  +    +  +" << endl;
		cout
			<< std::left << std::setfill(' ') << std::setw(3) << (int)*getLed(led + 30)
			<< std::left << std::setfill(' ') << std::setw(5) << (int)*getLed(led + 45)
			<< std::left << std::setfill(' ') << std::setw(3) << (int)*getLed(led)
			<< std::left << std::setfill(' ') << std::setw(3) << (int)*getLed(led + 15) << endl;
	}
	cout << "64 +    A1 A4\nMX 61   A2 A5\nPR 62   A3 A6\nG  63   N  G" << endl << endl;

}
