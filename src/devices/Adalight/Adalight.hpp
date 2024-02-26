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

#define ADALIGHT_NAME        "Adalight"
#define ADALIGHT_MAGIC       "Ada"
#define ADALIGHT_PRODUCT_IDS {"2e8a/1075", "838/8918", "ch341-uart"}

namespace LEDSpicer::Devices::Adalight {

/**
 * LEDSpicer::Devices::Adalight
 *
 * Adalight smart led controller (using WLED firmware on ESP8266)
 */
class Adalight : public DeviceSerial {

public:

	Adalight(umap<string, string>& options) :
	DeviceSerial(
		options,
		ADALIGHT_NAME
	) {}

	virtual ~Adalight() = default;

	virtual void drawHardwarePinMap();

	virtual void transfer() const;

protected:

	void detectPort() override;
};

} /* namespace */

deviceFactory(LEDSpicer::Devices::Adalight::Adalight)

#endif /* ADALIGHT_HPP_ */
