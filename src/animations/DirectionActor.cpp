/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DirectionActor.cpp
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

#include "DirectionActor.hpp"

using namespace LEDSpicer::Animations;

void DirectionActor::drawConfig() const {
	FrameActor::drawConfig();
	initDir.drawConfig();
}

void DirectionActor::restart() {
	FrameActor::restart();
	setDirection(initDir.getDirection());
	if (initDir.isBackward()) {
		stepping.frame = startAt
			? (stepping.frames * (100 - startAt)) / 100
			: stepping.getLastFrame();
	}
}

bool DirectionActor::isFirstFrame() const {
	return (
		(isForward()  and FrameActor::isFirstFrame()) or
		(isBackward() and FrameActor::isLastFrame())
	);
}

bool DirectionActor::isStartOfCycle() const {

	if (FrameActor::isStartOfCycle()) {
		if (initDir.isBouncer()) {
			if (isBouncing() and isBackward()) return true;
		}
		if (isForward()) return true;
		if (isStopped()) return true;
	}
	else if (FrameActor::isLastFrame() and stepping.step == 0) {
		if (initDir.isBouncer()) {
			if (isBouncing() and isForward()) return true;
		}
		if (isBackward()) return true;
		if (isStopped()) return true;
	}
	return false;
}

bool DirectionActor::isLastFrame() const {
	return (
		(FrameActor::isFirstFrame() and isBackward()) or
		(FrameActor::isLastFrame()  and isForward())
	);
}

bool DirectionActor::isEndOfCycle() const {
	if (not stepping.isLastStep()) return false;
	if (not initDir.isBouncer()) return isLastFrame();
	return isBouncing() and isLastFrame();
}

uint16_t DirectionActor::calculateNextOf(
	Direction&      currentDirection,
	const uint16_t  index,
	const Direction direction,
	const uint16_t  last
) {
	// Handle boundary conditions.
	if ((currentDirection.isForward() and index == last) or (not currentDirection.isForward() and index == 0)) {
		if (direction.isBouncer()) {
			currentDirection.reverse();
			return index;
		}
		// Non-bouncer: wrap to opposite end.
		return currentDirection.isForward() ? 0 : last;
	}

	// Normal movement.
	return currentDirection.isForward() ? index + 1 : index - 1;
}

uint16_t DirectionActor::nextOf(
	Direction       currentDirection,
	const uint16_t  index,
	const Direction direction,
	const uint16_t  amount
) {
	return calculateNextOf(currentDirection, index, direction, amount);
}

void DirectionActor::advanceFrame() {
	if (stepping.shouldMoveFrame())
		stepping.frame = calculateNextOf(*this, stepping.frame, initDir, stepping.frames - 1);
}

uint16_t DirectionActor::getFullFrames() const {
	return (FrameActor::getFullFrames() * (1 + initDir.isBouncer()));
}
