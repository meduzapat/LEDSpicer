/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DeviceSerial.hpp
 * @since     Aug 26, 2022
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2022 Patricio A. Rossi (MeduZa)
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

#include "Device.hpp"
#include "utility/Serial.hpp"

#ifndef DEVICESERIAL_HPP_
#define DEVICESERIAL_HPP_ 1

#define SERIAL_MAX_BOARDS 128

namespace LEDSpicer::Devices {

/**
 * LEDSpicer::Device::DeviceSerial
 *
 * Library to handle serial Device connections.
 */
class DeviceSerial : public Serial, public Device {

public:

	DeviceSerial(umap<string, string>& options, const string& name) :
		Device(Utility::parseNumber(options.count("leds") ? options["leds"] : "", "Invalid Value for number of leds") * 3, name),
		Serial(options.count("port") ? options["port"] : DEFAULT_SERIAL_PORT) {}

	virtual ~DeviceSerial() = default;

	virtual string getFullName() const;

protected:

	virtual void openHardware();

	virtual void closeHardware();
};

} /* namespace */

#endif /* DEVICESERIAL_HPP_ */
