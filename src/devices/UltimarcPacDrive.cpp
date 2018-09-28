/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file		UltimarcPacDrive.cpp
 * @since		Sep 19, 2018
 * @author		Patricio A. Rossi (MeduZa)
 * @copyright	Copyright © 2018 Patricio A. Rossi (MeduZa)
 */

#include "UltimarcPacDrive.hpp"

using namespace LEDSpicer::Devices;

UltimarcPacDrive::UltimarcPacDrive(uint8_t boardId, umap<string, string>& options) :
		Ultimarc(ULTIMARC_VENDOR, PAC_DRIVE_PRODUCT, PAC_DRIVE_LEDS) {
	board.name      = "Ultimarc Pac Drive Controller";
	board.vendor    = ULTIMARC_VENDOR;
	board.product   = PAC_DRIVE_PRODUCT;
	board.interface = 0;
	board.boardId   = boardId;
	board.value     = PAC_DRIVE_VALUE;
	board.LEDs      = PAC_DRIVE_LEDS;
	if (options.count("changePoint")) {
		try {
			changePoint = stoi(options["changePoint"]);
		}
		catch(...) {
			Log::warning("Invalid value for changePoint board " + getName() + " Id " + to_string(getId()));
		}
	}
}

void UltimarcPacDrive::drawHardwarePinMap() {
	uint8_t half = PAC_DRIVE_LEDS / 2;
	cout << getName() << " Pins " << PAC_DRIVE_LEDS << endl;
	cout << "Hardware pin map:" << endl <<
			"L     R"  << endl;
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

void UltimarcPacDrive::transfer() {

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

void UltimarcPacDrive::afterConnect() {}

