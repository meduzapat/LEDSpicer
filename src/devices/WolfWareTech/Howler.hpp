/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      HowlerController.hpp
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

#include "WolfWareTech.hpp"

#ifndef HOWLER_HPP_
#define HOWLER_HPP_ 1

#define HOWLER_NAME       "Howler"
#define HOWLER_PRODUCT    0x6800
#define HOWLER_WVALUE     0x00CE
#define HOWLER_INTERFACE  0
#define HOWLER_LEDS       96
#define HOWLER_MAX_BOARDS 4
#define HOWLER_IN_EP      0x02
#define HOWLER_OUT_EP     0x81

#define HOWLER_CMD_SET_GLOBAL_BRIGHTNESS 0x06
#define HOWLER_CMD_SET_RGB_LED           0x01
#define HOWLER_CMD_SET_INDIVIDUAL_LED    0x02

#define HOWLER_MSG(cmd, byte1, byte2) {HOWLER_WVALUE, cmd, byte1, byte2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}

namespace LEDSpicer {
namespace Devices {
namespace WolfWareTech {

/**
 * LEDSpicer::Devices::WolfWareTech::Howler
 */
class Howler: public WolfWareTech {

public:

	/**
	 * @param boardId
	 */
	Howler(uint8_t boardId, umap<string, string>& options) :
	WolfWareTech(
		HOWLER_WVALUE,
		HOWLER_INTERFACE,
		HOWLER_LEDS,
		HOWLER_MAX_BOARDS,
		boardId,
		HOWLER_NAME
	) {}

	virtual ~Howler() = default;

	virtual void drawHardwarePinMap();

	void transfer();

	uint16_t getProduct();

	virtual void resetLeds();
protected:

	virtual void transferToUSB(vector<uint8_t>& data);
};

} /* namespace WolfWareTech */
} /* namespace Devices */
} /* namespace LEDSpicer */

deviceFactory(LEDSpicer::Devices::WolfWareTech::Howler)

#endif /* HOWLER_HPP_ */
