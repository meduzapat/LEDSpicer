/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      LedWiz32.cpp
 * @since     Nov 7, 2018
 * @author    Patricio A. Rossi (MeduZa)
 * @copyright Copyright © 2018 - 2019 Patricio A. Rossi (MeduZa)
 */

#include "LedWiz32.hpp"

using namespace LEDSpicer::Devices::GroovyGameGear;

void LedWiz32::resetLeds() {
	setLeds(0);
	transfer();
}

void LedWiz32::afterClaimInterface() {
	// This will initialize the 4 controllers and set the pulse to 1.
	vector<uint8_t> data = {64, 255, 255, 255, 255, 1, 0, 0};
	transferData(data);
	Device::afterClaimInterface();
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
			std::left << std::setfill(' ') << std::setw(3) << (int)*getLed(r) << "   " <<
			std::left << std::setfill(' ') << std::setw(3) << (int)*getLed(l) << endl;
	}
	cout << endl;
}

void LedWiz32::transfer() {
	/*
	 * This device transfers chunks of 8 bits from 0 to 63.
	 * 0 to 48 with modulation.
	 * 49 to 63 without.
	 */
	vector<uint8_t> load;
	for (auto l : LEDs) {
		load.push_back(63 * (l / 255.00));
		if (load.size() == 8) {
			transferData(load);
			load.clear();
		}
	}
}

uint16_t LedWiz32::getProduct() {
	return LEDWIZ32_PRODUCT + board.boardId - 1;
}
