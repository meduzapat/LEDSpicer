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

#include "Error.hpp"

#pragma once

#define DrawDirection(d) (d == Directions::Forward ? "→" : "←")

namespace LEDSpicer::Utilities {

/**
 * LEDSpicer::Direction
 * Helper class with direction options.
 */
class Direction {

public:

	/**
	 * Direction enum values.
	 */
	enum class Directions : uint8_t {
		Stop,
		Forward,
		Backward
	};

	Direction() = default;

	/**
	 * Creates a new Direction from a string.
	 * @param direction
	 * @param bounce If true, the direction will bounce back when reaching the end.
	 */
	Direction(const string& direction, bool bounce = false) : Direction(str2direction(direction), bounce) {}

	/**
	 * Creates a new Direction with a Directions value.
	 * @param direction
	 * @param bounce If true, the direction will bounce back when reaching the end.
	 */
	Direction(const Directions& direction, bool bounce = false);

	virtual ~Direction() = default;

	/**
	 * Converts a direction into string.
	 * @param direction
	 * @return
	 */
	static const string direction2str(const Directions direction);

	/**
	 * Converts a string into a direction.
	 * @param direction
	 * @return
	 */
	static Directions str2direction(const string& direction);

	/**
	 * @return Returns the opposite direction.
	 */
	Directions getOppositeDirection();

	/**
	 * @return true if the direction is forward or forward with bouncing.
	 */
	virtual bool isForward() const;

	/**
	 * @return true if the direction is backward or backward with bouncing.
	 */
	virtual bool isBackward() const;

	/**
	 * @return true if not going in any direction.
	 */
	virtual bool isStopped() const;

	/**
	 * Reverses the current direction and bounce.
	 */
	void reverse();

	/**
	 * Returns the direction.
	 * @return
	 */
	Directions getDirection() const;

	/**
	 * Change the direction.
	 * @param dir
	 */
	void setDirection(const Directions dir);

	/**
	 * Returns if bouncing.
	 * @return
	 */
	bool isBouncer() const;

	/**
	 * Change bouncer mode.
	 * @param bounce
	 */
	void setBouncer(bool bounce);

	/**
	 * Draws the current configuration.
	 */
	void drawConfig() const;

protected:

	/// The direction to go.
	Directions direction = Directions::Forward;

	/// If true, the direction will bounce back when reaching the end.
	bool bounce = false;

};

} // namespace
