/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      PacDrive.cpp
 * @since     Sep 19, 2018
 * @author    Patricio A. Rossi (MeduZa)
 * @copyright Copyright © 2018 - 2019 Patricio A. Rossi (MeduZa)
 */

#include "PacDrive.hpp"

using namespace LEDSpicer::Devices::Ultimarc;

void PacDrive::resetLeds() {
	setLeds(0);
	transfer();
}

PacDrive::PacDrive(uint8_t boardId, umap<string, string>& options) :
	Ultimarc(
		PAC_DRIVE_WVALUE,
		PAC_DRIVE_INTERFACE,
		PAC_DRIVE_LEDS,
		PAC_DRIVE_MAX_BOARDS,
		boardId,
		PAC_DRIVE_NAME
	) {

	if (options.count("changePoint"))
		changePoint = Utility::parseNumber(
			options["changePoint"],
			"Invalid value for changePoint device " PAC_DRIVE_NAME " Id " + to_string(boardId)
		);
}

void PacDrive::drawHardwarePinMap() {
	uint8_t half = PAC_DRIVE_LEDS / 2;
	cout
		<< getFullName() << " Pins " << PAC_DRIVE_LEDS << endl
		<< "Hardware pin map:" << endl << "L     R"  << endl;
	for (uint8_t r = 0; r < half; ++r) {
		uint8_t l = PAC_DRIVE_LEDS - r - 1;
		setLed(r, r + 1);
		setLed(l, l + 1);
		cout <<
			std::left << std::setfill(' ') << std::setw(3) << (int)*getLed(r) <<
			"   " <<
			std::left << std::setfill(' ') << std::setw(3) << (int)*getLed(l) << endl;
	}
	cout << endl;
}

void PacDrive::transfer() {

	vector<uint8_t> load = {0, 0, 0, 0};

	for (uint8_t led = 0; led < PAC_DRIVE_LEDS; ++led) {
		if (LEDs[led] > changePoint) {
			// two groups of 8 bits.
			if (led < 9)
				load[3] |= 1 << led;
			else
				load[2] |= 1 << (led - 9);
		}
	}
	transferData(load);
}

uint16_t PacDrive::getProduct() {
	return PAC_DRIVE_PRODUCT + board.boardId - 1;
}
