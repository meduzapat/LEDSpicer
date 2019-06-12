/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      GroovyGameGear.hpp
 * @since     Nov 7, 2018
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2018 - 2019 Patricio A. Rossi (MeduZa)
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

#include "devices/Device.hpp"

#ifndef GGG_HPP_
#define GGG_HPP_ 1

/// GGG USB vendor code.
#define GGG_VENDOR 0xFAFA

namespace LEDSpicer {
namespace Devices {
namespace GroovyGameGear {

/**
 * LEDSpicer::Devices::GroovyGameGear
 * GGG family data and definitions.
 */
class GroovyGameGear: public Device {

public:

	using Device::Device;

	uint16_t getVendor() {
		return GGG_VENDOR;
	}

};

} /* namespace GroovyGameGear */
} /* namespace Devices */
} /* namespace LEDSpicer */

#endif /* GGG_HPP_ */
