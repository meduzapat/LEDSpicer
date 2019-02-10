/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      PacLed64.cpp
 * @since     Feb 2, 2019
 * @author    Patricio A. Rossi (MeduZa)
 * @copyright Copyright © 2019 Patricio A. Rossi (MeduZa)
 */

#include "PacLed64.hpp"

using namespace LEDSpicer::Devices::Ultimarc;

// TODO: if this is the same function for io, nano, and this, move this code to ultimarc class
void PacLed64::transfer() {
	vector<uint8_t> load;
	load.push_back(0x04);
	load.insert(load.end(), LEDs.begin(), LEDs.end());
	transferData(load);
}

uint16_t PacLed64::getProduct() {
	return PAC_LED64_PRODUCT + board.boardId - 1;
}

void PacLed64::drawHardwarePinMap() {

}

void PacLed64::afterClaimInterface() {
	vector<uint8_t> data = {0x03, 0, 0, 0xC0, 0};
	transferData(data);
	Device::afterClaimInterface();
}
