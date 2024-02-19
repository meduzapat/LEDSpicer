/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Colorful.hpp
 * @since     Nov 2, 2020
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

#include "Colors.hpp"

#ifndef COLORFUL_HPP_
#define COLORFUL_HPP_ 1


namespace LEDSpicer {

/**
 * LEDSpicer::Inputs::Colorful
 */
class Colorful: public Colors {

public:

	Colorful(const string& colors) : Colors(colors) {};

	virtual ~Colorful() = default;

	/**
	 * Moves to the next color
	 */
	void advanceColor();

	/**
	 * @return The next color.
	 */
	const Color* getCurrentColor();

	/**
	 * Reset to the 1st color.
	 */
	void reset();

protected:

	/// Current color index from the list of colors.
	uint8_t currentColor = 0;

};

} /* namespace LEDSpicer */

#endif /* COLORFUL_HPP_ */
