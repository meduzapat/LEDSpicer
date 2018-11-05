/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      UltimarcUltimate.cpp
 * @since     Jun 23, 2018
 * @author    Patricio A. Rossi (MeduZa)
 * @copyright Copyright © 2018 Patricio A. Rossi (MeduZa)
 */

#include "UltimarcUltimate.hpp"

using namespace LEDSpicer::Devices;

UltimarcUltimate::UltimarcUltimate(uint8_t boardId, umap<string, string>& options) :
		Ultimarc(ULTIMARC_REQUEST_TYPE, ULTIMARC_REQUEST, IPAC_ULTIMATE_LEDS)
	{
	// TODO: move this to a generic place where every board should populate their levels.
	if (not Utility::verifyValue(boardId, 1, IPAC_ULTIMAGE_MAX_BOARDS, false))
		throw Error("Board id should be a number between 1 and " + to_string(IPAC_ULTIMAGE_MAX_BOARDS));
	board.interface = 0;
	board.boardId   = boardId;
	board.value     = IPAC_ULTIMATE_VALUE;
}

UltimarcUltimate::~UltimarcUltimate() {
	setLeds(0);
	transfer();
}

void UltimarcUltimate::afterConnect() {

	// Detect interface.
	LogDebug("Detecting interface");

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
		LogInfo("No Game Controller mode detected");
		board.interface = IPAC_ULTIMATE_NGC_INTERFACE;
	}
	else {
		LogInfo("Game Controller mode detected");
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

uint16_t UltimarcUltimate::getProduct() {
	return IPAC_ULTIMATE_PRODUCT;
}

string UltimarcUltimate::getName() {
	return IPAC_ULTIMATE_NAME;
}
