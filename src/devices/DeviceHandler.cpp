/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DeviceHandler.cpp
 * @since     Dec 7, 2018
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

#include "DeviceHandler.hpp"

using namespace LEDSpicer::Devices;

DeviceHandler::DeviceHandler(const string& deviceName) :
	Handler(DEVICES_DIR + deviceName + ".so"),
	createFunction(reinterpret_cast<Device*(*)(uint8_t, umap<string, string>&)>(dlsym(handler, "createDevice"))),
	destroyFunction(reinterpret_cast<void(*)(Device*)>(dlsym(handler, "destroyDevice")))
{
	if (char *errstr = dlerror())
		throw Error("Failed to load device " + deviceName + " " + errstr);
}

DeviceHandler::~DeviceHandler() {

	// Destroy Devices.
	for (Device* device : devices) {
#ifdef DEVELOP
		LogDebug(device->getFullName() + " instance deleted");
#endif
		destroyFunction(device);
	}
}

Device* DeviceHandler::createDevice(uint8_t boardId, umap<string, string>& options) {
	Device* device = createFunction(boardId, options);
	devices.push_back(device);
	return device;
}
