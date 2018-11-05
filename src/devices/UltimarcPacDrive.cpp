/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      UltimarcPacDrive.cpp
 * @since     Sep 19, 2018
 * @author    Patricio A. Rossi (MeduZa)
 * @copyright Copyright © 2018 Patricio A. Rossi (MeduZa)
 */

#include "UltimarcPacDrive.hpp"

using namespace LEDSpicer::Devices;

UltimarcPacDrive::UltimarcPacDrive(uint8_t boardId, umap<string, string>& options) :
		Ultimarc(ULTIMARC_REQUEST_TYPE, ULTIMARC_REQUEST, PAC_DRIVE_LEDS) {
	// TODO: move this to a generic place where every board should populate their levels.
	if (not Utility::verifyValue(boardId, 1, PAC_DRIVE_MAX_BOARDS, false))
			throw Error("Device id should be a number between 1 and " + to_string(PAC_DRIVE_MAX_BOARDS));
	board.interface = PAC_DRIVE_INTERFACE;
	board.boardId   = boardId;
	board.value     = PAC_DRIVE_VALUE;
	if (options.count("changePoint"))
		changePoint = Utility::parseNumber(options["changePoint"], "Invalid value for changePoint device " + getName() + " Id " + to_string(getId()));
}

void UltimarcPacDrive::drawHardwarePinMap() {
	uint8_t half = PAC_DRIVE_LEDS / 2;
	cout
		<< getName() << " Pins " << PAC_DRIVE_LEDS << endl
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

uint16_t UltimarcPacDrive::getProduct() {
	return PAC_DRIVE_PRODUCT;
}

string UltimarcPacDrive::getName() {
	return IPAC_DRIVE_NAME;
}

void UltimarcPacDrive::afterConnect() {}

