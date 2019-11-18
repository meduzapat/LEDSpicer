/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      StepActor.cpp
 * @since     Jul 23, 2019
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2019 Patricio A. Rossi (MeduZa)
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
StepActor::StepActor(
	umap<string, string>& parameters,
	Group* const group,
	const vector<string>& requiredParameters
) :
	DirectionActor(parameters, group, requiredParameters),
	totalStepFrames(totalFrames / 2)
{
	totalFrames = group->size() - 1;
	restart();
}

void StepActor::drawConfig() {
	cout <<
		"Speed: " << speed2str(speed) << endl <<
		"Direction: " << direction2str(direction) << endl <<
		"Total Frames: " << static_cast<uint>(getTotalSteps()) <<
		" (" << static_cast<float>(getTotalSteps()) / FPS << " sec)"<< endl;
}

uint16_t StepActor::getTotalSteps() const {
	return totalStepFrames * totalFrames;
}

uint16_t StepActor::getCurrentStep() const {
	return (currentFrame * totalStepFrames) - currentStepFrame;
}

void StepActor::advanceFrame() {
	if (currentStepFrame == totalStepFrames) {
		currentStepFrame = 0;
		DirectionActor::advanceFrame();
		return;
	}
	++currentStepFrame;
}

void StepActor::restart() {
	currentStepFrame = 0;
	DirectionActor::restart();
}

bool StepActor::isFirstFrame() const {
	return (not currentStepFrame and DirectionActor::isFirstFrame());
}

bool StepActor::isLastFrame() const {
	return (currentStepFrame == totalStepFrames and DirectionActor::isLastFrame());
}

void StepActor::changeFrameElement(const Color& color, bool fade) {

	float percent = PERCENT(currentStepFrame, totalStepFrames);

	if (fade)
		changeElementColor(currentFrame, color.fade(100 - percent), filter);
	else
		changeElementColor(currentFrame, color, filter);

	Directions dir = cDirection;
	uint8_t next = calculateNextOf(dir, currentFrame, direction, totalFrames);
	changeElementColor(next, color.fade(percent), filter);
}

void StepActor::changeFrameElement(const Color& color, const Color& colorNext) {

	float percent = PERCENT(currentStepFrame, totalStepFrames);

	changeElementColor(currentFrame, colorNext.transition(color, percent), filter);

	Directions dir = cDirection;
	uint8_t next = calculateNextOf(dir, currentFrame, direction, totalFrames);
	changeElementColor(next, colorNext.fade(percent), filter);
}
