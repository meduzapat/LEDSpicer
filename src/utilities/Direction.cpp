/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Direction.cpp
 * @since     May 27, 2019
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

#include "Direction.hpp"

using namespace LEDSpicer::Utilities;

Direction::Direction(const Directions& direction, bool bounce) :
	direction(direction),
	bounce(bounce)
{
	// Stop does not moves.
	if (direction == Directions::Stop) this->bounce = false;
}

const string Direction::direction2str(const Directions direction) {
	switch (direction) {
	case Directions::Stop:
		return "Stop";
	case Directions::Forward:
		return "Forward";
	case Directions::Backward:
		return "Backward";
	}
	return "";
}

Direction::Directions Direction::str2direction(const string& direction) {
	if (direction == "Stop")
		return Directions::Stop;
	if (direction == "Forward")
		return Directions::Forward;
	if (direction == "Backward")
		return Directions::Backward;
	throw Error("Invalid direction ") << direction;
}

Direction::Directions Direction::getOppositeDirection() const {
	switch (direction) {
	case Directions::Forward:
		return Directions::Backward;
	case Directions::Backward:
		return Directions::Forward;
	default:
	case Directions::Stop:
		return Directions::Stop;
	}
}

bool Direction::isForward() const {
	return direction == Directions::Forward;
}

bool Direction::isBackward() const {
	return direction == Directions::Backward;
}

bool Direction::isStopped() const {
	return direction == Directions::Stop;
}

void Direction::reverse() {
	direction = getOppositeDirection();
	bounce    = not bounce;
}

Direction::Directions Direction::getDirection() const {
	return direction;
}

void Direction::setDirection(const Directions direction) {
	this->direction = direction;
}

bool Direction::isBouncer() const {
	return bounce;
}

void Direction::setBouncer(const bool bounce) {
	this->bounce = bounce;
}

void Direction::drawConfig() const {
	cout << "Direction: " << direction2str(direction);
	if (bounce) cout << " Bouncer";
	cout << endl;
}
