/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Ultimarc.hpp
 * @since     Sep 27, 2018
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

#include "devices/DeviceUSB.hpp"

#ifndef ULTIMARC_HPP_
#define ULTIMARC_HPP_ 1

namespace LEDSpicer::Devices::Ultimarc {

/**
 * Ultimarc family data and definitions.
 */
class Ultimarc : public DeviceUSB {

public:

	using DeviceUSB::DeviceUSB;

	uint16_t getVendor() const {
		return ULTIMARC_VENDOR;
	}

protected:

	virtual void afterClaimInterface() {}

};

} /* namespace */

#endif /* ULTIMARC_HPP_ */
