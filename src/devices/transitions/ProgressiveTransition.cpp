/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      ProgressiveTransition.cpp
 * @since     Sep 21, 2025
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2018 - 2025 Patricio A. Rossi (MeduZa)
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

#include "ProgressiveTransition.hpp"

using namespace LEDSpicer::Devices::Transitions;

bool ProgressiveTransition::run() {
#ifdef DEVELOP
	if (Log::isLogging(LOG_DEBUG)) {
		cout << "Transition " << progress << "%" << endl;
	}
#endif
	calculate();
	progress += increase;
	return progress < 100;
}

void ProgressiveTransition::drawConfig() const {
	cout << "Speed: " << speed2str(speed) << endl;
}

float ProgressiveTransition::calculateSpeed(const Speeds speed) {
	float durationSec;
	switch (speed) {
	case Speeds::VeryFast: durationSec = 0.5f; break;
	case Speeds::Fast:     durationSec = 1.0f; break;
	default:
	case Speeds::Normal:   durationSec = 1.5f; break;
	case Speeds::Slow:     durationSec = 2.0f; break;
	case Speeds::VerySlow: durationSec = 2.5f; break;
	}

	return 100.0f / (durationSec * Actor::getFPS());
}

void ProgressiveTransition::reset() {
	Transition::reset();
	progress = 0;
}
