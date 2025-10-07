/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Pulse.cpp
 * @since     Jun 24, 2018
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2018 - 2026 Patricio A. Rossi (MeduZa)
 *
 * @copyright LEDSpicer is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * @copyright LEDSpicer is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * @copyright You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "Pulse.hpp"

using namespace LEDSpicer::Animations;

actorFactory(Pulse)

Pulse::Pulse(StringUMap& parameters, Group* const layout) :
	DirectionActor(parameters, layout, REQUIRED_PARAM_ACTOR_PULSE),
	Colorful(parameters.exists("color") ? parameters["color"] : parameters.exists("colors") ? parameters["colors"] : ""),
	mode(parameters.exists("mode") ? str2mode(parameters["mode"]) : Modes::Exponential)
{
	stepping.frames = calculateFramesBySpeed(speed);
}

void Pulse::calculateElements() {
	const Color* color = colors[currentColor];
	float c;
	if (mode == Modes::Linear)
		c = stepping.frame;
	else
		c = static_cast<float>(stepping.frame * stepping.frame) / stepping.frames;
#ifdef DEVELOP
	if (Log::isLogging(LOG_DEBUG)) {
	cout <<
		"Pulse: " << DrawDirection(cDirection.getDirection())  <<
		" Frame " << (stepping.frame + 1)                      <<
		" = "     << PERCENT(c, stepping.frames)               <<
		"% "      << color->getName()                          << endl;
	}
#endif
	changeElementsColor(color->fade(PERCENT(c, stepping.frames)), filter);
	if (isEndOfCycle()) advanceColor();
}

void Pulse::drawConfig() const {
	cout <<
		"Type: Pulse " << endl <<
		"Colors: ";
	this->drawColors();
	cout << endl << "Mode: " << mode2str(mode) << endl;
	DirectionActor::drawConfig();
}

string Pulse::mode2str(const Modes mode) {
	switch (mode) {
	case Modes::Linear:      return "Linear";
	case Modes::Exponential: return "exponential";
	}
	return "";
}

Pulse::Modes Pulse::str2mode(const string& mode) {
	if (mode == "Linear")      return Modes::Linear;
	if (mode == "Exponential") return Modes::Exponential;
	throw Error("Invalid mode ") << mode;
}

void Pulse::restart() {
	Colorful::reset();
	DirectionActor::restart();
}
