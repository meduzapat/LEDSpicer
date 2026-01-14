/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Monochromatic.cpp
 * @since     Aug 27, 2022
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2022 Patricio A. Rossi (MeduZa)
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

#include "Monochromatic.hpp"

using namespace LEDSpicer::Utilities;

Monochromatic::Monochromatic(StringUMap& options, const string& deviceName) {
	if (options.exists("changePoint")) {
		int n(Utility::parseNumber(
			options["changePoint"],
			"Invalid value for changePoint " + deviceName
		));
		Utility::verifyValue<int>(n, 10, 245, true);
		changePoint = n;
	}
}

uint8_t Monochromatic::getChangePoint() const {
	return changePoint;
}
