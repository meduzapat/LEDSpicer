/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Adalight.hpp
 * @since     Jan 10, 2021
 * @author    Bzzrd
 *
 * @copyright Copyright © 2021 Bzzrd
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

#include "devices/DeviceSerial.hpp"
#include "utility/Utility.hpp"

#ifndef ADALIGHT_HPP_
#define ADALIGHT_HPP_ 1

#define ADALIGHT_NAME "Adalight"

namespace LEDSpicer {
namespace Devices {
namespace Adalight {

/**
 * LEDSpicer::Devices::Adalight
 *
 * Adalight smart led controller (using WLED firmware on ESP8266)
 */
class Adalight : public DeviceSerial {

public:

	Adalight(uint8_t  boardId, umap<string, string>& options) :
		DeviceSerial(
			ADALIGHT_NAME,
			Utility::parseNumber(options.count("leds") ? options["leds"] : "", "Invalid Value for number of leds") * 3,
			options
		) {}

	virtual ~Adalight() = default;

	virtual void resetLeds();

	string getFullName() const;

	virtual void drawHardwarePinMap();

	virtual void transfer() const;

protected:

	void openDevice();

};

} /* namespace Devices */
} /* namespace LEDSpicer */
} /* namespace AdaLight */
deviceFactory(LEDSpicer::Devices::Adalight::Adalight)

#endif /* ADALIGHT_HPP_ */
