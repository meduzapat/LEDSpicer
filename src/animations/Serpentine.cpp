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

	std::fill(internalValues.begin(), internalValues.end(), 0);

	group.elements[frame].setColor(color);
	if (tailLength) {
		uint8_t tailElem = frame ? frame - 1 : group.count();
		for (uint8_t c = 0; c < tailLength; c++, tailElem--) {
			group.elements[tailElem].setColor(tailColor.fade(tailweight * (tailLength - c)));
			if (tailElem == 0)
				tailElem = group.count();
		}
	}
}

void Serpentine::drawConfig() {
	cout << "Type: Serpentine " << endl;
	Actor::drawConfig();
	cout << "Color: ";
	color.drawColor();
	cout << " Tail Color: ";
	tailColor.drawColor();
	cout <<
		"Direction: " << direction2str(direction) << endl << std::dec <<
		"Tail Length: " << (int)tailLength << endl;
}
