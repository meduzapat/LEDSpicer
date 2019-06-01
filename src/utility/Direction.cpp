/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Direction.cpp
 * @since     May 27, 2019
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

#include "Direction.hpp"

using namespace LEDSpicer;

Direction::Direction(string direction) {
	if (not direction.empty())
		this->direction = str2direction(direction);
}

Direction::Directions Direction::str2direction(const string& direction) {

	if (direction == "Stop")
		return Directions::Stop;
	if (direction == "Forward")
		return Directions::Forward;
	if (direction == "Backward")
		return Directions::Backward;
	if (direction == "ForwardBouncing")
		return Directions::ForwardBouncing;
	if (direction == "BackwardBouncing")
		return Directions::BackwardBouncing;
	throw Error("Invalid direction " + direction);
}

string Direction::direction2str(Directions direction) {
	switch (direction) {
	case Directions::Stop:
		return "None";
	case Directions::Forward:
		return "Forward";
	case Directions::Backward:
		return "Backward";
	case Directions::ForwardBouncing:
		return "Forward Bouncing";
	case Directions::BackwardBouncing:
		return "Backward Bouncing";
	}
	return "";
}
