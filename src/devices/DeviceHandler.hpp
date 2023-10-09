/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DeviceHandler.hpp
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

#include "Handler.hpp"
#include "Device.hpp"

#ifndef DEVICEHANDLER_HPP_
#define DEVICEHANDLER_HPP_ 1

namespace LEDSpicer::Devices {

/**
 * LEDSpicer::Devices::DeviceHandler
 * This is a Device factory to loader and create Device plugins.
 */
class DeviceHandler : public Handler {

public:

	DeviceHandler() = default;

	/**
	 * @see Handler::Handler()
	 */
	DeviceHandler(const string& deviceName);

	virtual ~DeviceHandler();

	/**
	 * Creates a device calling the create function and keep record of it.
	 * @see createFunction pointer.
	 */
	Device* createDevice(umap<string, string>& options);

protected:

	/// List of created devices.
	vector<Device*> devices;

	/**
	 * Pointer to the plugin's creation function.
	 * @param plugin parameters.
	 * @return a new created plugin.
	 */
	Device*(*createFunction)(umap<string, string>&) = nullptr;

	/**
	 * Pointer to the plugin's destruction function.
	 * @param pointer to the plugin to destroy.
	 */
	void(*destroyFunction)(Device*) = nullptr;
};

} /* namespace */

#endif /* DEVICEHANDLER_HPP_ */
