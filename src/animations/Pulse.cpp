/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file		Pulse.cpp
 * @since		Jun 24, 2018
 * @author		Patricio A. Rossi (MeduZa)
 * @copyright	Copyright © 2018 Patricio A. Rossi (MeduZa)
 */

#include "Pulse.hpp"

using namespace LEDSpicer::Animations;

void Pulse::calculateFrame() {

}

void Pulse::drawConfig() {
	cout << "Type: Pulse " << endl;
	Animation::drawConfig();
	cout << "Color: #" << std::uppercase << std::hex <<
			std::setfill('0') << std::setw(2) << (int)color.getR() <<
			std::setfill('0') << std::setw(2) << (int)color.getG() <<
			std::setfill('0') << std::setw(2) << (int)color.getB() << std::dec << endl;
}
