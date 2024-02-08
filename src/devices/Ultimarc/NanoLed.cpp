/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      NanoLed.cpp
 * @since     Mar 11, 2019
 * @author    Patricio A. Rossi (MeduZa)
 * @copyright Copyright © 2018 - 2024 Patricio A. Rossi (MeduZa)
 */

#include "NanoLed.hpp"

using namespace LEDSpicer::Devices::Ultimarc;

uint16_t NanoLed::getProduct() const {
	return NANO_LED_PRODUCT + boardId - 1;
}

void NanoLed::drawHardwarePinMap() {

	cout << getFullName() << " Pins " << NANO_LED_LEDS << endl << "Hardware pin map:" << endl;
	for (uint8_t led = 0; led < NANO_LED_LEDS; ++led) {
		setLed(led, led + 1);
		cout << std::left << std::setfill(' ') << std::setw(3) << static_cast<int>(*getLed(led)) << endl;
	}
	cout << "This device only have 1 connector with 60 LEDs in series."<< endl;
}
