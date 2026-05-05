/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      FrameActor.cpp
 * @since     Jul 18, 2019
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

#include "FrameActor.hpp"

using namespace LEDSpicer::Animations;

FrameActor::FrameActor(
	StringUMap& parameters,
	Group* const group,
	const vector<string>& requiredParameters
) :
	Actor(parameters, group, requiredParameters),
	Speed(parameters["speed"]),
	startAt(parameters.exists("startAt") ? Utility::parseNumber(parameters["startAt"], "Invalid Value for start at") : 0),
	cycles(parameters.exists("cycles")   ? Utility::parseNumber(parameters["cycles"],  "Invalid Value for cycles")   : 0)
{
	Utility::verifyValue<uint16_t>(startAt, 0, 100);
}

void FrameActor::drawConfig() const {
	Actor::drawConfig();
	cout << "Frame time:     " << std::fixed << std::setprecision(2) << (getFullFrames() / FPS) << " sec" << endl;
	if (cycles)
		cout << "Will run for:   " << cycles << " Cycles" << endl;
	if (startAt)
		cout << "Start at Frame: " << calculateStartFrame() << " (" << startAt << "%)" << endl;
	Speed::drawConfig();
}

bool FrameActor::isFirstFrame() const {
	return stepping.frame == 0;
}

bool FrameActor::isStartOfCycle() const {
	return stepping.frame == 0 and stepping.step == 0;
}

bool FrameActor::isLastFrame() const {
	return stepping.isLastFrame();
}

bool FrameActor::isEndOfCycle() const {
	return stepping.isLastStep() and stepping.isLastFrame();
}

void FrameActor::draw() {
	Actor::draw();
	advanceFrame();
}

void FrameActor::restart() {
	Actor::restart();
	if (startAt) {
		stepping.frame = calculateStartFrame();
#ifdef DEVELOP
		LogDebug("Starting Actor" + std::to_string(actorNumber) + " from frame " + to_string(stepping.frame));
#endif
	}
	else {
		stepping.frame = 0;
	}
	cycle         = 0;
	stepping.step = 0;
}

bool FrameActor::isRunning() {

	// Check timers.
	if (not Actor::isRunning()) return false;

	// Check cycles, whatever kicks 1st.
	if (cycles) {
		if (cycle >= cycles) return false;
		if (isEndOfCycle()) {
			++cycle;
#ifdef DEVELOP
			if (cycle == cycles)
				LogDebug("Actor " + std::to_string(actorNumber) + " completed after " + to_string(cycles) + " cycles");
#endif
		}
	}
	return true;
}

uint16_t FrameActor::getFullFrames() const {
	return stepping.steps ? stepping.frames * stepping.steps : stepping.frames;
}

float FrameActor::getRunTime() const {
	float
		actorTime  = Actor::getRunTime(),
		fullFrames = static_cast<float>(getFullFrames()),
		baseTime   = cycles ? (fullFrames * cycles / FPS) : fullFrames / FPS;
	return baseTime > actorTime ? actorTime : baseTime;
}

uint16_t FrameActor::calculateStepsBySpeed(Speeds speed) {
	switch (speed) {
	case Speeds::VeryFast: return 0;
	case Speeds::Fast:     return FPS * 0.2;
	default:
	case Speeds::Normal:   return FPS * 0.4;
	case Speeds::Slow:     return FPS * 0.6;
	case Speeds::VerySlow: return FPS * 0.8;
	}
}

uint16_t FrameActor::calculateFramesBySpeed(Speeds speed) {
	switch (speed) {
	case Speeds::VeryFast: return FPS / 3;
	case Speeds::Fast:     return FPS / 2;
	default:
	case Speeds::Normal:   return FPS;
	case Speeds::Slow:     return FPS * 2;
	case Speeds::VerySlow: return FPS * 3;
	}
}

void FrameActor::advanceFrame() {
	stepping.advanceStep();
}

uint16_t FrameActor::calculateStartFrame()  const noexcept {
	return (stepping.frames * (startAt - 1)) / 100.00f;
}
