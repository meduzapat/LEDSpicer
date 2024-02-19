/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Time.cpp
 * @since     Feb 26, 2020
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2018 - 2024 Patricio A. Rossi (MeduZa)
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

#include "Time.hpp"

using namespace LEDSpicer;

Time::Time(uint16_t seconds) {
#ifdef DEVELOP
	LogDebug("Setting Clock for " + std::to_string(seconds) + " seconds.");
#endif
	clockTime = std::chrono::system_clock::now() + std::chrono::seconds(seconds);
}

bool Time::isTime() {
	return (std::chrono::system_clock::now() > clockTime);
}
