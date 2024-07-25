/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      StepActor.cpp
 * @since     Jul 23, 2019
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2018 - 2024 Patricio A. Rossi (MeduZa)
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
	DirectionActor(parameters, group, requiredParameters)
{
	totalFrames = group->size() - 1;
	// Default frames calculation.
	switch (speed) {
	case Speeds::VeryFast:
		setTotalStepFrames(0);
		break;
	case Speeds::Fast:
		setTotalStepFrames(FPS * 0.2);
		break;
	case Speeds::Normal:
		setTotalStepFrames(FPS * 0.4);
		break;
	case Speeds::Slow:
		setTotalStepFrames(FPS * 0.6);
		break;
	case Speeds::VerySlow:
		setTotalStepFrames(FPS * 0.8);
		break;
	}
}

void StepActor::drawConfig() {
	cout << "Steps: " << static_cast<uint>(totalStepFrames) << endl;
	DirectionActor::drawConfig();
}

uint16_t StepActor::getCurrentStep() const {
	if (not totalStepFrames)
		return currentFrame;
	if (currentFrame)
		return (currentFrame * totalStepFrames) - currentStepFrame;
	return 0;
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
	if (totalStepFrames)
		return (not currentStepFrame and DirectionActor::isFirstFrame());
	return DirectionActor::isFirstFrame();
}

bool StepActor::isLastFrame() const {
	if (totalStepFrames)
		return (currentStepFrame == totalStepFrames and DirectionActor::isLastFrame());
	return DirectionActor::isLastFrame();
}

void StepActor::changeFrameElement(uint8_t index, const Color& color, Directions direction) {

	float percent = stepPercent * currentStepFrame;

	if (direction == Directions::Backward)
		percent = 100 - percent;

#ifdef DEVELOP
	cout << "Element " << static_cast<uint>(index) << " faded " << percent << "%";
#endif

	changeElementColor(index, color.fade(percent), filter);
}

void StepActor::changeFrameElement(const Color& color, Directions direction) {
	changeFrameElement(currentFrame, color, direction);
}

void StepActor::changeFrameElement(const Color& color, bool fade, Directions direction) {

	float percent = stepPercent * currentStepFrame;
	uint8_t next  = nextOf(cDirection, currentFrame, direction, totalFrames);

#ifdef DEVELOP
	cout << "Frame " << to_string(currentFrame) << " to " << to_string(next) << " " << percent << "%" << endl;
#endif

	if (fade)
		changeElementColor(currentFrame, color.fade(100 - percent), filter);
	else
		changeElementColor(currentFrame, color, filter);

	changeElementColor(next, color.fade(percent), filter);
}

void StepActor::changeFrameElement(const Color& color, const Color& colorNext, Directions direction) {

	float percent = stepPercent * currentStepFrame;
	uint8_t next  = nextOf(cDirection, currentFrame, direction, totalFrames);

#ifdef DEVELOP
	cout << "Frame " << to_string(currentFrame) << " to " << to_string(next) << " " << percent << "%" << endl;
#endif

	changeElementColor(currentFrame, colorNext.transition(color, percent), filter);
	changeElementColor(next, colorNext.fade(percent), filter);
}

void StepActor::setTotalStepFrames(uint8_t totalStepFrames) {
	this->totalStepFrames = totalStepFrames;
	stepPercent = totalStepFrames ? PERCENT(1.0, totalStepFrames) : 0;
}

const uint16_t StepActor::getFullFrames() const {
	if (totalStepFrames)
		return totalStepFrames * totalFrames * (1 + isBouncer());
	return (totalFrames * (1 + isBouncer()));
}
