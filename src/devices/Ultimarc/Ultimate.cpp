/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      UltimarcUltimate.cpp
 * @since     Jun 23, 2018
 * @author    Patricio A. Rossi (MeduZa)
 * @copyright Copyright © 2018 Patricio A. Rossi (MeduZa)
 */

#include "Ultimate.hpp"

using namespace LEDSpicer::Devices::Ultimarc;

void Ultimate::afterConnect() {

	// Detect interface.
	LogDebug("Detecting interface");

	libusb_device *device = libusb_get_device(handle);

	if (!device)
		throw Error("Unable to retrieve device information");

	libusb_device_descriptor descriptor;

	uint8_t value = 0;

	libusb_get_device_descriptor(device, &descriptor);

	// Detect Game Controller mode.
	if ((descriptor.bcdDevice & 0x40)) {
		LogInfo("No Game Controller mode detected");
		board.interface = IPAC_ULTIMATE_NGC_INTERFACE;
	}
	else {
		LogInfo("Game Controller mode detected");
		board.interface = IPAC_ULTIMATE_INTERFACE;
	}
}

void Ultimate::afterClaimInterface() {
	vector<uint8_t> data = {0x03, 0, 0, 0xC0, 0};
	transferData(data);
	Device::afterClaimInterface();
}

void Ultimate::drawHardwarePinMap() {
	uint8_t
		half    = IPAC_ULTIMATE_LEDS / 2,
		fillerL = 1,
		fillerR = half + 1,
		rLed    = half;
	cout << getFullName() << " Pins " << IPAC_ULTIMATE_LEDS << endl;
	cout << "Hardware pin map:"    << endl <<
			"R  G  B     B  G  R"  << endl;
	for (uint8_t lLed = 0; lLed < half; ++lLed) {
		setLed(lLed, fillerL++);
		setLed(fillerR - 1, fillerR);
		fillerR++;
		cout << std::left << std::setfill(' ') << std::setw(3) << (int)*getLed(lLed);
		if (not ((lLed + 1) % 3)) {
			cout << "   ";
			for (uint8_t c = 0; c < 3; ++c)
				cout << std::left << std::setfill(' ') << std::setw(3) << (int)*getLed(rLed++);
			cout << endl;
		}
	}
	cout << endl;
}

void Ultimate::transfer() {

	vector<uint8_t> load;
	load.push_back(0x04);
	load.insert(load.end(), LEDs.begin(), LEDs.end());
	transferData(load);
}

uint16_t Ultimate::getProduct() {
	return IPAC_ULTIMATE_PRODUCT + board.boardId - 1;
}
