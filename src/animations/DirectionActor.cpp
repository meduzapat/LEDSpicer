/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DirectionActor.cpp
 * @since     Jul 18, 2019
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2018 - 2020 Patricio A. Rossi (MeduZa)
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

DirectionActor::DirectionActor(
	umap<string, string>& parameters,
	Group* const group,
	const vector<string>& requiredParameters
) :
	FrameActor(parameters, group, requiredParameters),
	Direction(parameters["direction"]),
	cDirection(direction)
{
	if (not isDirectionForward())
		currentFrame = totalFrames;
}

void DirectionActor::drawConfig() {
	cout << "Direction: " << direction2str(direction) << endl;
	FrameActor::drawConfig();
}

bool DirectionActor::isBouncing() const {
	switch (direction) {
	case Directions::ForwardBouncing:
		return cDirection == Directions::Backward;
	case Directions::BackwardBouncing:
		return cDirection == Directions::Forward;
	}
	return false;
}

bool DirectionActor::isBouncer() const {
	return direction == Directions::ForwardBouncing or direction == Directions::BackwardBouncing;
}

bool DirectionActor::isFirstFrame() const {
	switch (direction) {
	case Directions::Forward:
	case Directions::ForwardBouncing:
		if (cDirection == Directions::Forward and currentFrame == 0)
			return true;
		break;
	case Directions::Backward:
	case Directions::BackwardBouncing:
		if (cDirection == Directions::Backward and currentFrame == totalFrames)
			return true;
		break;
	}
	return false;
}

bool DirectionActor::isLastFrame() const {
	switch (direction) {
	case Directions::Forward:
	case Directions::BackwardBouncing:
		if (cDirection == Directions::Forward and currentFrame == totalFrames)
			return true;
		break;
	case Directions::Backward:
	case Directions::ForwardBouncing:
		if (cDirection == Directions::Backward and currentFrame == 0)
			return true;
		break;
	}
	return false;
}

bool DirectionActor::isDirectionForward() const {
	return direction == Directions::Forward or direction == Directions::ForwardBouncing;
}

bool DirectionActor::isDirectionBackward() const {
	return direction == Directions::Backward or direction == Directions::BackwardBouncing;
}

void DirectionActor::restart() {
	if (isDirectionForward()) {
		currentFrame = 0;
		cDirection = Directions::Forward;
	}
	else {
		currentFrame = totalFrames;
		cDirection = Directions::Backward;
	}
}

uint8_t DirectionActor::calculateNextOf(
	Directions& currentDirection,
	uint8_t frame,
	Directions direction,
	uint8_t totalElements)
{

	switch (direction) {
	case Directions::Forward:
		if (frame == totalElements)
			frame = 0;
		else
			++frame;
		break;
	case Directions::ForwardBouncing:
	case Directions::BackwardBouncing:
		if (currentDirection == Directions::Forward) {
			// change of direction.
			if (frame == totalElements) {
				currentDirection = Directions::Backward;
				--frame;
				break;
			}
			++frame;
			break;
		}
		// change of direction.
		if (frame == 0) {
			currentDirection = Directions::Forward;
			++frame;
			break;
		}
		--frame;
		break;
	case Directions::Backward:
		if (frame == 0)
			frame = totalElements;
		else
			--frame;
		break;
	}

	return frame;
}

void DirectionActor::advanceFrame() {
	currentFrame = calculateNextOf(cDirection, currentFrame, direction, totalFrames);
}
