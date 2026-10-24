/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Ultimarc.hpp
 * @since     Sep 27, 2018
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2018 - 2026 Patricio A. Rossi (MeduZa)
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

#pragma once

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

	void afterClaimInterface() override {}

	/**
	 * Streams the LED buffer as a 0xFE reset followed by intensity pairs.
	 * Only the byte order is common between the boards that use this, the report framing is not,
	 * so every caller provides its own message and says where the command byte sits in it.
	 *
	 * @param message the device report framing.
	 * @param command index of the command byte inside the message, the value byte follows it.
	 */
	void transferPairs(vector<uint8_t> message, uint8_t command) const;

};

} // namespace
