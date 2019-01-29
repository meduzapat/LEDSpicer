/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DeviceHandler.hpp
 * @since     Dec 7, 2018
 * @author    Patricio A. Rossi (MeduZa)
 * @copyright Copyright © 2018 Patricio A. Rossi (MeduZa)
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

	virtual ~DeviceHandler() {}

	Device* createDevice(uint8_t boardId, umap<string, string>& options);

	void destroyDevice(Device* device);

protected:

	std::function<Device*(uint8_t, umap<string, string>&)> createFunction;

	std::function<void(Device*)> destroyFunction;
};

} /* namespace Devices */
} /* namespace LEDSpicer */

#endif /* DEVICEHANDLER_HPP_ */
