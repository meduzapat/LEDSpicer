/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      RaspberryPi.hpp
 * @since     Feb 15, 2020
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

#include "../Device.hpp"
#include <pigpio.h>

#ifndef RASPBERRYPI_HPP_
#define RASPBERRYPI_HPP_ 1

#define RPI_NAME "Raspberry Pi"
#define RPI_LEDS 28

namespace LEDSpicer {
namespace Devices {
namespace RaspberryPi {

/**
 * LEDSpicer::Devices::RaspberryPi::RaspberryPi
 *
 * Raspberry Pi GPIO ports.
 */
class RaspberryPi : public Device {

public:

	RaspberryPi(uint8_t boardId, umap<string, string>& options) : Device(RPI_LEDS, RPI_NAME) {}

	virtual ~RaspberryPi() = default;

	virtual void resetLeds();

	virtual string getFullName() const;

	virtual void drawHardwarePinMap();

	virtual void transfer() const;

protected:

	static bool initialized;

	virtual void openDevice();

	virtual void closeDevice();

	vector<uint8_t> usedleds;
};

} /* namespace RaspberryPi */
} /* namespace Devices */
} /* namespace LEDSpicer */

deviceFactory(LEDSpicer::Devices::RaspberryPi::RaspberryPi)

#endif /* RASPBERRYPI_HPP_ */
