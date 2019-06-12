/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DeviceHandler.hpp
 * @since     Dec 7, 2018
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2018 - 2019 Patricio A. Rossi (MeduZa)
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

#include "Handler.hpp"
#include "Device.hpp"
#include <functional>

#ifndef DEVICEHANDLER_HPP_
#define DEVICEHANDLER_HPP_ 1

namespace LEDSpicer {
namespace Devices {

/**
 * LEDSpicer::Devices::DeviceHandler
 */
class DeviceHandler : public Handler {

public:

	DeviceHandler() = default;

	DeviceHandler(const string& deviceName);

	virtual ~DeviceHandler() = default;

	Device* createDevice(uint8_t boardId, umap<string, string>& options);

	void destroyDevice(Device* device);

protected:

	std::function<Device*(uint8_t, umap<string, string>&)> createFunction;

	std::function<void(Device*)> destroyFunction;
};

} /* namespace Devices */
} /* namespace LEDSpicer */

#endif /* DEVICEHANDLER_HPP_ */
