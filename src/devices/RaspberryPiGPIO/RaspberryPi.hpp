/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      RaspberryPi.hpp
 * @since     Feb 15, 2020
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

#include "../Device.hpp"
#include <pigpio.h>

#ifndef RASPBERRYPI_HPP_
#define RASPBERRYPI_HPP_ 1

#define RPI_NAME     "Raspberry Pi"
#define RPI_LEDS     28
#define RPI_TRANSFER 1 // Individual.

namespace LEDSpicer::Devices::RaspberryPi {

/**
 * LEDSpicer::Devices::RaspberryPi::RaspberryPi
 *
 * Raspberry Pi GPIO ports.
 * This is a connectionless device.
 */
class RaspberryPi : public Device {

public:

	RaspberryPi(umap<string, string>& options) : Device(RPI_LEDS, RPI_NAME) {}

	virtual ~RaspberryPi() = default;

	void resetLeds() override;

	string getFullName() const override;

	void drawHardwarePinMap() override;

	void transfer() const override;

protected:

	/// Rpi can be initialized only once.
	static bool initialized;

	/// Keeps track of only used LEDs to be send.
	vector<uint8_t> usedleds;

	void openHardware() override;

	void closeHardware() override;
};

} /* namespace */

deviceFactory(LEDSpicer::Devices::RaspberryPi::RaspberryPi)

#endif /* RASPBERRYPI_HPP_ */
