/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DeviceHandler.cpp
 * @since     Dec 7, 2018
 * @author    Patricio A. Rossi (MeduZa)
 * @copyright Copyright © 2018 - 2019 Patricio A. Rossi (MeduZa)
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

Device* DeviceHandler::createDevice(uint8_t boardId, umap<string, string>& options) {
	return createFunction(boardId, options);
}

void DeviceHandler::destroyDevice(Device* device) {
	destroyFunction(device);
}
