/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Colors.cpp
 * @since     Jul 20, 2019
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

#include "Colors.hpp"

using namespace LEDSpicer;

void Colors::extractColors(string colors) {

	if (colors.empty()) {
		this->colors.reserve(Color::getNames().size());
		for (auto& c : Color::getNames())
			this->colors.push_back(&Color::getColor(c));
	}
	else {
		for (auto& c : Utility::explode(colors, ',')) {
			Utility::trim(c);
			this->colors.push_back(&Color::getColor(c));
		}
		this->colors.shrink_to_fit();
	}

	if (this->colors.size() < 2)
		throw Error("You need two or more colors for actor Random to do something.");

	if (this->colors.size() > UINT8_MAX)
		throw Error("Too many colors (" + to_string(this->colors.size()) + "), the maximum is " + to_string(UINT8_MAX) + " .");
}
