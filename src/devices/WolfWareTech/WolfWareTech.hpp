/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      WolfWareTech.hpp
 * @since     Feb 5, 2020
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2020 Patricio A. Rossi (MeduZa)
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

#include "devices/DeviceUSB.hpp"
#include "Brands.hpp"

#ifndef WOLFWARETECH_HPP_
#define WOLFWARETECH_HPP_ 1

namespace LEDSpicer {
namespace Devices {
namespace WolfWareTech {

/**
 * This is the Base Header for the WolfWare Tech controllers.
 * LEDSpicer::Devices::WolfWareTech::WolfWareTech
 */
class WolfWareTech : public DeviceUSB {

public:

	using DeviceUSB::DeviceUSB;

	uint16_t getVendor() const {
		return WOLFWARETECH_VENDOR;
	}

protected:

	virtual void afterConnect() {}

	virtual void afterClaimInterface() {}
};

} /* namespace WolfWareTech */
} /* namespace Devices */
} /* namespace LEDSpicer */

#endif /* WOLFWARETECH_HPP_ */
