/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Ultimate.hpp
 * @since     Jun 23, 2018
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

#include "Ultimarc.hpp"

#pragma once

#define IPAC_ULTIMATE_NAME       "Ultimarc Ipac Ultimate IO"
#define IPAC_ULTIMATE_PRODUCT    0x0410
#define IPAC_ULTIMATE_WVALUE     0x0203
#define IPAC_ULTIMATE_INTERFACE  2
#define IPAC_ULTIMATE_LEDS       96
#define IPAC_ULTIMAGE_MAX_BOARDS 4
#define IPAC_ULTIMAGE_TRANSFER   0// All.

#define ULTIMAGE_MSG(byte1, byte2) {0x03, 0, 0, byte1, byte2}

namespace LEDSpicer::Devices::Ultimarc {

/**
 * LEDSpicer::Devices::Ultimate
 * Class to keep Ultimate I/O data and functionality.
 */
class Ultimate : public Ultimarc {

public:

	/**
	 * @param options
	 */
	Ultimate(StringUMap& options) :
	Ultimarc(
		IPAC_ULTIMATE_WVALUE,
		0, // to be defined.
		IPAC_ULTIMATE_LEDS,
		IPAC_ULTIMAGE_MAX_BOARDS,
		options,
		IPAC_ULTIMATE_NAME
	) {}

	virtual ~Ultimate() = default;

	void drawHardwareLedMap() override;

	void transfer() const override;

	uint16_t getProduct() const override;

	void resetLeds() override;

protected:

	void afterConnect() override;

};

deviceFactory(Ultimate)

} // namespace
