/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Time.hpp
 * @since     Feb 26, 2020
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

// For ints.
#include <cstdint>
#include <chrono>
using std::chrono::system_clock;
#include "utility/Log.hpp"

#ifndef TIME_HPP_
#define TIME_HPP_ 1

namespace LEDSpicer {

/**
 * LEDSpicer::Time
 */
class Time {

public:

	Time(uint16_t seconds);

	virtual ~Time() = default;

	/**
	 * @return returns true when the time is over.
	 */
	bool isTime();

protected:

	/// Calculated timepoint to finish.
	system_clock::time_point clockTime;

};

} /* namespace LEDSpicer */

#endif /* TIME_HPP_ */
