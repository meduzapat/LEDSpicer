/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      LedWiz32.cpp
 * @since     Nov 7, 2018
 * @author    Patricio A. Rossi (MeduZa)
 * @copyright Copyright © 2018 Patricio A. Rossi (MeduZa)
 */

#include "LedWiz32.hpp"

using namespace LEDSpicer::Devices::LedWiz;

LedWiz32::~LedWiz32() {
	setLeds(0);
	transfer();
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
			std::left << std::setfill(' ') << std::setw(3) << (int)*getLed(r) <<
			"   " <<
			std::left << std::setfill(' ') << std::setw(3) << (int)*getLed(l) << endl;
	}
	cout << endl;
}

void LedWiz32::transfer() {

	vector<uint8_t> load;
	load.push_back(0x04);
	load.insert(load.end(), LEDs.begin(), LEDs.end());
	transferData(load);
}

uint16_t LedWiz32::getProduct() {
	return LEDWIZ32_PRODUCT + board.boardId - 1;
}
