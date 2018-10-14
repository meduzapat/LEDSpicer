/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file		UltimarcUltimate.cpp
 * @since		Jun 23, 2018
 * @author		Patricio A. Rossi (MeduZa)
 * @copyright	Copyright © 2018 Patricio A. Rossi (MeduZa)
 */

#include "UltimarcUltimate.hpp"

using namespace LEDSpicer::Devices;

UltimarcUltimate::UltimarcUltimate(uint8_t boardId, umap<string, string>& options) :
		Ultimarc(ULTIMARC_REQUEST_TYPE, ULTIMARC_REQUEST, IPAC_ULTIMATE_LEDS) {
	board.name      = "Ultimarc Ipac Ultimate IO";
	board.vendor    = ULTIMARC_VENDOR;
	board.product   = IPAC_ULTIMATE_PRODUCT;
	board.interface = 0;
	board.boardId   = boardId;
	board.value     = IPAC_ULTIMATE_VALUE;
	board.LEDs      = IPAC_ULTIMATE_LEDS;
}

void UltimarcUltimate::afterConnect() {

	// Detect interface.
	Log::debug("detecting interface");

	libusb_device *device = libusb_get_device(handle);

	if (!device)
		throw Error("Unable to retrieve device information");

	libusb_device_descriptor descriptor;

	uint8_t value = 0;

	libusb_get_device_descriptor(device, &descriptor);

	/*
	 * Not sure if this is correct but looks like is working fine,
	 * my theory is that when the board is on game controller mode
	 * another interface is listed in between, so the number increases by one.
	 * Also I not sure why 0x40.
	 */
	if ((descriptor.bcdDevice & 0x40)) {
		Log::info("No Game Controller mode detected");
		board.interface = IPAC_ULTIMATE_NGC_INTERFACE;
	}
	else {
		Log::info("Game Controller mode detected");
		board.interface = IPAC_ULTIMATE_INTERFACE;
	}
}

void UltimarcUltimate::afterClaimInterface() {
	vector<uint8_t> data = {0x03, 0, 0, 0xC0, 0};
	transferData(data);
	Device::afterClaimInterface();
}

void UltimarcUltimate::drawHardwarePinMap() {
	uint8_t
		half    = IPAC_ULTIMATE_LEDS / 2,
		fillerL = 1,
		fillerR = half + 1,
		rLed    = half;
	cout << getName() << " Pins " << IPAC_ULTIMATE_LEDS << endl;
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

void UltimarcUltimate::transfer() {

	vector<uint8_t> load;
	load.push_back(0x04);
	load.insert(load.end(), LEDs.begin(), LEDs.end());
	transferData(load);
}

/*
not used

void Device::setLedsTo(uint8_t intensity) {
	Log::debug("Turning LEDs " + to_string(intensity));
	uint8_t map[] = {0x03, code, value, value2, 0};
	transfer({0x03, 0x80, intensity, 0xC0, 0});
}

void Device::setLedTo(uint8_t led, uint8_t intensity) {
	Log::debug("Turning LED " + to_string(led) + " to " + to_string(intensity));
	transfer({0x03, led, intensity, 0xC0, 0});
}
*/

