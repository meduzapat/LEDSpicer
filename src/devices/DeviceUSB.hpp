/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DeviceUSB.hpp
 *
 * @since     Feb 11, 2020
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

#include "Device.hpp"
#include "utility/USB.hpp"

#ifndef DEVICEUSB_HPP_
#define DEVICEUSB_HPP_ 1
namespace LEDSpicer {
namespace Devices {

/**
 * LEDSpicer::Devices::Device
 * Generic Device settings and functionality.
 */
class DeviceUSB : public USB, public Device {

public:

	DeviceUSB(
		uint16_t wValue,
		uint8_t  interface,
		uint8_t  elements,
		uint8_t  maxBoards,
		uint8_t  boardId,
		const string& name
	) :
		Device(elements, name),
		USB(wValue, interface, boardId, maxBoards) {}

	virtual ~DeviceUSB() = default;

	/**
	 * @return the device name with Id.
	 */
	virtual string getFullName();

protected:

	virtual void connect();

	virtual void openDevice();

	virtual void closeDevice();

};

}} /* namespace LEDSpicer */

#endif /* DEVICEUSB_HPP_ */
