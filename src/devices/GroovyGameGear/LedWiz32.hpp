/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      LedWiz32.hpp
 * @since     Nov 7, 2018
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

#include "GroovyGameGear.hpp"
#include <chrono>
#include <thread>

#ifndef LEDWIZ32_HPP_
#define LEDWIZ32_HPP_ 1

#define LEDWIZ32_NAME          "Led-Wiz 32"
#define LEDWIZ32_PRODUCT       0x00f0
#define LEDWIZ32_WVALUE        0x0200
#define LEDWIZ32_INTERFACE     0
#define LEDWIZ32_LEDS          32
#define LEDWIZ32_MAX_BOARDS    16
#define LEDWIZ_WAIT            1000 //microseconds
#define LEDWIZ32_TRANSFER      2 // Batches.

namespace LEDSpicer::Devices::GroovyGameGear {

/**
 * LEDSpicer::Devices::GroovyGameGear::LedWiz32
 *
 * Led-Wiz 32 controller class.
 */
class LedWiz32 : public GroovyGameGear {

public:

	LedWiz32(umap<string, string>& options) :
	GroovyGameGear(
		LEDWIZ32_WVALUE,
		LEDWIZ32_INTERFACE,
		LEDWIZ32_LEDS,
		LEDWIZ32_MAX_BOARDS,
		options,
		LEDWIZ32_NAME
	) {}

	virtual ~LedWiz32() = default;

	void drawHardwarePinMap() override;

	void transfer() const override;

	uint16_t getProduct() const override;

protected:

	static uint8_t dumpFrame;

	void afterClaimInterface() override;
};

deviceFactory(LedWiz32)

} /* namespace */


#endif /* LEDWIZ32_HPP_ */
