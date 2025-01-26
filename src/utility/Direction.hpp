/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Direction.hpp
 * @since     May 27, 2019
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

#include <cstdint>
#include <string>
#include <iostream>

#include "Error.hpp"

#ifndef DIRECTION_HPP_
#define DIRECTION_HPP_ 1

#define DrawDirection(d) (d == Directions::Forward ? "→" : "←")

namespace LEDSpicer {

using std::string;
using std::cout;
using std::endl;

/**
 * LEDSpicer::Direction
 * Helper class with direction options.
 */
class Direction {

public:

	enum class Directions : uint8_t {Stop, Forward, Backward, ForwardBouncing, BackwardBouncing};

	Direction() = default;

	Direction(string direction);

	virtual ~Direction() = default;

	static string direction2str(const Directions direction);

	static Directions str2direction(const string& direction);

	static Directions getOppositeDirection(const Directions direction);

	void drawConfig() const;

protected:

	Directions direction  = Directions::Forward;

};

} /* namespace LEDSpicer */

#endif /* DIRECTION_HPP_ */
