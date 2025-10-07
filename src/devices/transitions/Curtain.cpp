/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Curtain.cpp
 * @since     Sep 25, 2025
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

#include "Curtain.hpp"

using namespace LEDSpicer::Devices::Transitions;

bool Curtain::run() {
	if (actor->isBouncing())
		to->runFrame();
	else
		current->runFrame();
	actor->draw();
	return actor->isRunning();
}

void Curtain::drawConfig() const {
	cout << "Transition: Curtain" << endl;
	ActorDriven::drawConfig();
}

string Curtain::closingWays2str(const ClosingWays closingWays) {
	switch (closingWays) {
	case ClosingWays::Left:  return "Left";
	case ClosingWays::Right: return "Right";
	default: return "Both";
	}
}

Curtain::ClosingWays Curtain::str2ClosingWays(const string& closingWays) {
	if (closingWays == "Left")  return ClosingWays::Left;
	if (closingWays == "Right") return ClosingWays::Right;
	return ClosingWays::Both;
}
