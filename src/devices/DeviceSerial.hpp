/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Serial.hpp
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

// To handle terminal.
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>

#ifndef SERIAL_HPP_
#define SERIAL_HPP_ 1

#define DEFAULT_SERIAL_PORT "/dev/ttyUSB0"
namespace LEDSpicer {
namespace Devices {


/**
 * LEDSpicer::Serial
 *
 * Library to handle serial connections.
 */
class DeviceSerial : public Device {

public:

	DeviceSerial(const string& name, uint8_t leds, umap<string, string>& options) :
		Device(leds, name),
		port(options.count("port") ? options["port"] : DEFAULT_SERIAL_PORT) {}

	virtual ~DeviceSerial() = default;

	virtual string getFullName() const;

protected:

	termios params;

	const string port;

	int fd = 0;

	virtual void openDevice();

	virtual void closeDevice();

};

}} /* namespace LEDSpicer */

#endif /* SERIAL_HPP_ */
