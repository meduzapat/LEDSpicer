/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Pulse.cpp
 * @since     Jun 24, 2018
 * @author    Patricio A. Rossi (MeduZa)
 * @copyright Copyright © 2018 Patricio A. Rossi (MeduZa)
 */

#include "Pulse.hpp"

using namespace LEDSpicer::Animations;

const vector<bool> Pulse::calculateElements() {
	float c = static_cast<float>(currentActorFrame);
	c = round((c * c) / totalActorFrames);
#ifdef DEVELOP
	cout << "frame: " << to_string(currentActorFrame) << " = " << to_string(c) << endl;
#endif
	changeElementsColor(color.fade(c * 100 / totalActorFrames), filter);
	return affectedElements;
}

void Pulse::drawConfig() {
	cout << "Actor Type: Pulse " << endl;
	Actor::drawConfig();
	cout << "Color: ";
	color.drawColor();
	cout << endl;
}
