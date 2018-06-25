/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file		Serpentine.cpp
 * @since		Jun 22, 2018
 * @author		Patricio A. Rossi (MeduZa)
 * @copyright	Copyright © 2018 Patricio A. Rossi (MeduZa)
 */

#include "Serpentine.hpp"

using namespace LEDSpicer::Animations;

void Serpentine::calculateFrame() {

	uint8_t c  = 0;
	uint8_t c2 = 2;
/*
	c  = 0;
	c2 = 2;
	ipacUltimate.setLeds(0);
	ipacUltimate.transfer();
	while (c2--) {
		for (c = 1; c <= 96; c++) {
			if (not running)
				break;
			if (c > 6)
				ipacUltimate.setLed(c - 6, 0);
			else
				ipacUltimate.setLed(97 - c, 0);
			ipacUltimate.setLed(c, 255);
			ipacUltimate.transfer();
		}
	}
*/
}

void Serpentine::drawConfig() {
	cout << "Type: Serpentine " << endl;
	Animation::drawConfig();
	cout << "Color: #" << std::uppercase << std::hex <<
		std::setfill('0') << std::setw(2) << (int)color.getR() <<
		std::setfill('0') << std::setw(2) << (int)color.getG() <<
		std::setfill('0') << std::setw(2) << (int)color.getB() <<
		" Tail Color: #" <<
		std::setfill('0') << std::setw(2) << (int)tailColor.getR() <<
		std::setfill('0') << std::setw(2) << (int)tailColor.getG() <<
		std::setfill('0') << std::setw(2) << (int)tailColor.getB() << endl <<
		"Direction: " << direction2str(direction) << endl << std::dec <<
		"Tail Length: " << (int)tailLength << " Tail Effect " << effects2str(tailEffect) << endl;
}
