/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Pulse.cpp
 * @since     Jun 24, 2018
 * @author    Patricio A. Rossi (MeduZa)
 * @copyright Copyright © 2018 Patricio A. Rossi (MeduZa)
 */

#include "Pulse.hpp"

using namespace LEDSpicer::Animations;

void Pulse::calculateElements() {
	changeElementsColor(color.fade((static_cast<uint>(currentActorFrame) * 100) / totalActorFrames), filter);
}

void Pulse::drawConfig() {
	cout << "Actor Type: Pulse " << endl;
	Actor::drawConfig();
	cout << "Color: ";
	color.drawColor();
	cout << endl;
}
