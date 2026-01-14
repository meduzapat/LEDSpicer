/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      StepActor.cpp
 * @since     Jul 23, 2019
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

#include "StepActor.hpp"

using namespace LEDSpicer::Animations;

void StepActor::changeFrameElement(uint16_t index, const Color& color, Directions direction) {

	float percent = stepPercent * stepping.step;
	if (direction == Directions::Backward) percent = 100 - percent;

#ifdef DEVELOP
	if (Log::isLogging(LOG_DEBUG)) {
		cout << "Element " << index + 1 << " faded " << percent << "%" << endl;
	}
#endif

	changeElementColor(index, color.fade(percent), filter);
}

void StepActor::changeFrameElement(const Color& color, Directions direction) {
	changeFrameElement(stepping.frame, color, direction);
}

void StepActor::changeFrameElement(const Color& color, bool fade, Directions direction) {
	auto [percent, next] = changeFrameElementCommon(direction);
	if (fade) {
		changeElementColor(stepping.frame, color.fade(100 - percent), filter);
		changeElementColor(next, color.fade(percent), filter);
	}
	else {
		changeElementColor(stepping.frame, color, filter);
	}
}

void StepActor::changeFrameElement(const Color& color, const Color& colorNext, Directions direction) {
	auto [percent, next] = changeFrameElementCommon(direction);
	changeElementColor(stepping.frame, colorNext.transition(color, percent), filter);
	changeElementColor(next, colorNext.fade(percent), filter);
}

void StepActor::calculateStepPercent() {
	stepPercent = stepping.steps ? PERCENT(1.0f, stepping.steps) : 0;
}

StepActor::FrameTransition StepActor::changeFrameElementCommon(const Directions& direction) const {

	float percent = stepPercent * stepping.step;
	// This is fake.
	uint16_t next = nextOf(cDirection, stepping.frame, direction, stepping.frames - 1);
	// Next is fake and will bounce late due to incorrect direction.
	if (isBouncer() and (
			(stepping.frame == 0    and cDirection.isBackward()) or
			(stepping.isLastFrame() and cDirection.isForward())
		)
	) {
		percent = 100;
		next    = stepping.frame;
	}

#ifdef DEVELOP
	if (Log::isLogging(LOG_DEBUG)) {
		cout
			<< "Frame " << std::setw(2) << static_cast<uint16_t>(stepping.frame + 1)
			<< " to "   << std::setw(2) << static_cast<uint16_t>(next + 1)
			<< " Step " << std::setw(1) << static_cast<uint16_t>(stepping.step + 1)
			<< " to "   << std::setw(1) << static_cast<uint16_t>(stepping.steps)
			<< " "      << std::setw(6) << std::fixed << std::setprecision(2) << percent << "%" << endl;
	}
#endif
	return {percent, next};
}

