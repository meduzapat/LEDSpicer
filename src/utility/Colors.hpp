/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Colors.hpp
 * @since     Jul 20, 2019
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

#include "Utility.hpp"
#include "Color.hpp"

#ifndef COLORS_HPP_
#define COLORS_HPP_ 1

namespace LEDSpicer {

/**
 * LEDSpicer::Colors
 *
 * Represent a group of Color.
 */
class Colors {

public:

	Colors() = default;

	Colors(const string& colors);

	virtual ~Colors() = default;

	void drawColors() const;

protected:

	vector<const Color*> colors;

	/**
	 * Creates an array of colors from a string of comma separated color names.
	 * @param colors
	 */
	void extractColors(string colors);

};

} /* namespace LEDSpicer */

#endif /* COLORS_HPP_ */
