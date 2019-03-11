/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      TimedActor.cpp
 * @since     Sep 16, 2018
 * @author    Patricio A. Rossi (MeduZa)
 * @copyright Copyright © 2018 - 2019 Patricio A. Rossi (MeduZa)
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
