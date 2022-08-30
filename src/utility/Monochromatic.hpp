/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Monochromatic.hpp
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

#include "utility/Log.hpp"
#include "utility/Utility.hpp"

#ifndef MONOCHROMATIC_HPP_
#define MONOCHROMATIC_HPP_ 1

namespace LEDSpicer {

/**
 * LEDSpicer::Monochromatic
 *
 * Class to be used by devices that only can do ON/OFF
 */
class Monochromatic {

public:

	Monochromatic(umap<string, string>& options, const string& deviceName);

	virtual ~Monochromatic() = default;

protected:

	// This provides the change point where the black (Off) become white (On)

	uint8_t changePoint = 64;
};

} /* namespace LEDSpicer */

#endif /* MONOCHROMATIC_HPP_ */
