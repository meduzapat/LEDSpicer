/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Speed.hpp
 * @since     May 27, 2019
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

#include <cstdint>
#include <string>
#include <iostream>

#include "Error.hpp"

#ifndef SPEED_HPP_
#define SPEED_HPP_ 1

namespace LEDSpicer {

using std::string;
using std::cout;
using std::endl;

/**
 * LEDSpicer::Speed
 * Helper class with speed options.
 */
class Speed {

public:

	enum class Speeds : uint8_t {VeryFast, Fast, Normal, Slow, VerySlow};

	Speed() = default;

	Speed(string speed);

	virtual ~Speed() = default;

	static string speed2str(Speeds speed);

	static Speeds str2speed(const string& speed);

	void drawConfig();

protected:

	/// The current speed.
	Speeds speed = Speeds::Normal;
};

} /* namespace LEDSpicer */

#endif /* SPEED_HPP_ */
