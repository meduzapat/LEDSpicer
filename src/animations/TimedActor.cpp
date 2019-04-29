/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      TimedActor.cpp
 * @since     Sep 16, 2018
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2018 - 2019 Patricio A. Rossi (MeduZa)
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

#include "TimedActor.hpp"

using namespace LEDSpicer::Animations;

TimedActor::TimedActor(umap<string, string>& parameters, Group* const group, const vector<string>& requiredParameters) :
	Actor(parameters, group, requiredParameters)
{
	changeFrameTotal = static_cast<uint8_t>(speed) + 1;
}

bool TimedActor::isSameFrame() const {
	return changeFrame != 1;
}

bool TimedActor::willChange() const {
	return changeFrame == changeFrameTotal;
}

void TimedActor::changeFrameElement(const Color& color, bool fade) {

	uint8_t index = currentActorFrame - 1;

	if (speed == Speed::VeryFast) {
		changeElementColor(index, color, filter);
		return;
	}

	affectedElements[index] = true;
	float percent = (changeFrame - 1) * 100.00 / changeFrameTotal;
	if (fade)
		changeElementColor(index, color.fade(100 - percent), filter);
	else
		changeElementColor(index, color, filter);

	Directions dir = cDirection;
	uint8_t next = calculateNextOf(dir, currentActorFrame, direction, totalActorFrames);
	changeElementColor(next - 1, color.fade(percent), filter);
	affectedElements[next - 1] = true;
}

void TimedActor::changeFrameElement(const Color& color, const Color& colorNext) {

	uint8_t index = currentActorFrame - 1;

	if (speed == Speed::VeryFast) {
		changeElementColor(index, colorNext, filter);
		return;
	}

	affectedElements[index] = true;
	float percent = (changeFrame - 1) * 100.00 / changeFrameTotal;
	changeElementColor(index, colorNext.transition(color, percent), filter);

	Directions dir = cDirection;
	uint8_t next = calculateNextOf(dir, currentActorFrame, direction, totalActorFrames);
	changeElementColor(next - 1, colorNext.fade(percent), filter);
	affectedElements[next - 1] = true;
}

void TimedActor::advanceActorFrame() {
	if (willChange()) {
		changeFrame = 1;
		Actor::advanceActorFrame();
		return ;
	}
	changeFrame++;
}
