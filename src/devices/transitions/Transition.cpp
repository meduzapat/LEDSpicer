/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Transition.cpp
 * @since     Sep 20, 2025
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

#include "Transition.hpp"

using namespace LEDSpicer::Devices::Transitions;

Transition::Transition(Profile* current) : current(current) {
	current->stopInputs();
}

void Transition::setTarget(Profile* to) {
	this->to = to;
	reset();
}

bool Transition::run() {
	return false;
}

void Transition::drawConfig() const {
	cout << "Transition: None" << endl;
}

void Transition::reset() {
	if (to) to->reset();
}

Transition::Effects Transition::str2effect(const string& effect) {
	if (effect == "None")      return Effects::None;
	if (effect == "FadeOutIn") return Effects::FadeOutIn;
	if (effect == "Crossfade") return Effects::Crossfade;
	if (effect == "Curtain")   return Effects::Curtain;
	return Effects::None;
}

string Transition::effect2str(Effects effect) {
	switch (effect) {
	default:
	case Effects::None:      return "None";
	case Effects::FadeOutIn: return "FadeOutIn";
	case Effects::Crossfade: return "Crossfade";
	case Effects::Curtain:   return "Curtain";
	}
}
