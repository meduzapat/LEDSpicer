/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Serial.cpp
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

#include "DeviceSerial.hpp"

using namespace LEDSpicer::Devices;


void DeviceSerial::openDevice() {
	LogInfo("Opening connection to " + port);
	if ((fd = open(port.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK)) < 0)
		throw Error("Can't open device " + getFullName());
}
void DeviceSerial::closeDevice() {
	close(fd);
}

string DeviceSerial::getFullName() const {
	return "Device: " + name + " at " + port;
}
