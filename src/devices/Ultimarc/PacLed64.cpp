/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      PacLed64.cpp
 * @since     Feb 2, 2019
 * @author    Patricio A. Rossi (MeduZa)
 * @copyright Copyright © 2019 Patricio A. Rossi (MeduZa)
 */

#include "PacLed64.hpp"

using namespace LEDSpicer::Devices::Ultimarc;

void PacLed64::resetLeds() {

	// Set Off Ramp Speed.
	vector<uint8_t> data LED64_MSG(0xC0, 0);
	transferData(data);

	// Turn off all LEDs and internal buffer.
	setLeds(0);
	data[3] = 0x80;
	transferData(data);
}

// TODO: if this is the same function for io, nano, and this, move this code to ultimarc class
void PacLed64::transfer() {

	// Send FE00 command.
	vector<uint8_t> data LED64_MSG(0xFE, 0);
	transferData(data);

	// Send 32 pairs.
	for (uint8_t c = 0; c < LEDs.size(); c+=2) {
		data.clear();
		data.push_back(LEDs[c]);
		data.push_back(LEDs[c + 1]);
		transferData(data);
	}
}

uint16_t PacLed64::getProduct() {
	return PAC_LED64_PRODUCT + board.boardId - 1;
}

void PacLed64::drawHardwarePinMap() {

	cout << getFullName() << " Pins " << PAC_LED64_LEDS << endl << "Hardware pin map:" << endl;
	for (uint8_t led = 0; led < PAC_LED64_LEDS; ++led)
		setLed(led, led + 1);
	for (uint8_t led = 0; led < 15; ++led) {
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
