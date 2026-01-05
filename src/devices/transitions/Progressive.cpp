/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Progressive.cpp
 * @since     Sep 22, 2025
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

#include "Progressive.hpp"

using namespace LEDSpicer::Devices::Transitions;

Progressive::Progressive(const string& speed) : Speed(speed) {
	switch (this->speed) {
	case Speeds::VeryFast:
		step = 16;
		break;
	case Speeds::Fast:
		step = 8;
		break;
	case Speeds::Normal:
		step = 4;
		break;
	case Speeds::Slow:
		step = 2;
		break;
	case Speeds::VerySlow:
		step = 1;
		break;
	}
}

bool Progressive::run() {
	if (progress > COMPLETE) return false;
	calculate();
	progress += step;
	return true;
}

void Progressive::drawConfig() const {
	Speed::drawConfig();
}

void Progressive::reset() {
	Transition::reset();
	progress = 0;
}
