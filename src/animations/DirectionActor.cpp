/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DirectionActor.cpp
 * @since     Jul 18, 2019
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

#include "DirectionActor.hpp"

using namespace LEDSpicer::Animations;

void DirectionActor::drawConfig() const {
	Direction::drawConfig();
	FrameActor::drawConfig();
}

void DirectionActor::restart() {
	FrameActor::restart();
	if (not isForward()) stepping.frame = stepping.getLastFrame();
}

bool DirectionActor::isBouncing() const {
	return cDirection.isBouncer();
}

bool DirectionActor::isFirstFrame() const {
	return (
		(cDirection.isForward()  and FrameActor::isFirstFrame()) or
		(cDirection.isBackward() and FrameActor::isLastFrame())
	);
}

bool DirectionActor::isStartOfCycle() const {

	if (FrameActor::isStartOfCycle()) {
		if (isBouncer()) {
			if (isBouncing() and cDirection.isBackward()) return true;
		}
		if (cDirection.isForward()) return true;
		if (cDirection.isStopped()) return true;
	}
	else if (FrameActor::isLastFrame() and stepping.step == 0) {
		if (isBouncer()) {
			if (isBouncing() and cDirection.isForward()) return true;
		}
		if (cDirection.isBackward()) return true;
		if (cDirection.isStopped()) return true;
	}
	return false;
}

bool DirectionActor::isLastFrame() const {
	return (
		(FrameActor::isFirstFrame() and cDirection.isBackward()) or
		(FrameActor::isLastFrame()  and cDirection.isForward())
	);
}

bool DirectionActor::isEndOfCycle() const {
	if (not stepping.isLastStep()) return false;
	if (not isBouncer()) return isLastFrame();
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
		stepping.frame = calculateNextOf(cDirection, stepping.frame, *this, stepping.frames -1);
}

uint16_t DirectionActor::getFullFrames() const {
	return (FrameActor::getFullFrames() * (1 + isBouncer()));
}
