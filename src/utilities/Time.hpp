/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Time.hpp
 * @since     Feb 26, 2020
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

#include "Defaults.hpp"

#pragma once

namespace LEDSpicer::Utilities {

/**
 * LEDSpicer::Time
 *
 * Timer class to be used to set expirations.
 */
class Time {

public:

	/**
	 * Creates an uninitialized clock.
	 */
	Time() = default;

	/**
	 * Creates an object using seconds
	 * @param seconds
	 */
	Time(float seconds);

	virtual ~Time() = default;

	/**
	 * @return returns true when the time is over.
	 */
	bool isTime() const;

	/**
	 * Resets the clock to milliseconds.
	 * @param milliseconds
	 */
	void reset(uint milliseconds);

	/**
	 * Sets the point where a frame started.
	 */
	static void setFrameTime();

protected:

	/// Shared frame anchor — all timers use this as their "now".
	inline static system_clock::time_point frameTime = system_clock::now();

	/// Calculated timepoint to finish.
	system_clock::time_point timeOnExpiration = {};

};

} // namespace
