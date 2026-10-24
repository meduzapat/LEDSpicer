/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      FF00SharedCode.cpp
 * @since     Apr 7, 2019
 * @author    Patricio A. Rossi (MeduZa)
 * @copyright Copyright © 2018 - 2026 Patricio A. Rossi (MeduZa)
 */

#include "FF00SharedCode.hpp"

using namespace LEDSpicer::Devices::Ultimarc;


void FF00SharedCode::resetLeds() {

	// Set Off Ramp Speed.
	vector<uint8_t> data FF00_MSG(0xC0, 0);
	transferToConnection(data);

	// Turn off all LEDs and internal buffer.
	setLeds(0);
	data[FF00_MSG_COMMAND] = 0x80;
	transferToConnection(data);
}

void FF00SharedCode::transfer() const {
	transferPairs(FF00_MSG(0, 0), FF00_MSG_COMMAND);
}
