/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file		UltimarcUltimate.cpp
 * @since		Jun 23, 2018
 * @author		Patricio A. Rossi (MeduZa)
 * @copyright	Copyright © 2018 Patricio A. Rossi (MeduZa)
 */

#include "UltimarcUltimate.hpp"

using namespace LEDSpicer::Devices;

UltimarcUltimate::UltimarcUltimate(uint8_t boardId, uint16_t fps) : Device() {
	board.name      = "Ipac Ultimate IO";
	board.vendor    = IPAC_ULTIMATE_VENDOR;
	board.product   = IPAC_ULTIMATE_PRODUCT;
	board.interface = 0;
	board.boardId   = boardId;
	board.value     = IPAC_ULTIMATE_VALUE;
	board.LEDs      = IPAC_ULTIMATE_LEDS;
	// convert FPS to milliseconds.
	board.fps       = 1000/fps;
	// Initialize the internal LED matrix and add extra field for the command.
	LEDs.resize(IPAC_ULTIMATE_LEDS + 1);
	LEDs.shrink_to_fit();
	LEDs[0] = 0x04;
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
		Log::info("No Game Controller detected");
		board.interface = IPAC_ULTIMATE_NGC_INTERFACE;
	}
	else {
		Log::info("Game Controller detected");
		board.interface = IPAC_ULTIMATE_INTERFACE;
	}
}

void UltimarcUltimate::afterClaimInterface() {
	Log::debug("Initializing board");
	transfer({0x03, 0, 0, 0xC0, 0});
}

void UltimarcUltimate::drawHardwarePinMap() {
	uint8_t
		half    = getNumberOfLeds() / 2,
		fillerL = 1,
		fillerR = half + 1;
	cout << getName() << " Pins " << (int)getNumberOfLeds() << endl;
	cout << "Hardware pin map:" << endl <<
			"R  G  B     B  G  R"  << endl;
	for (uint8_t c = 1; c <= getNumberOfLeds() / 2; ++c) {
		*getLED(c)        = fillerL++;
		*getLED(c + half) = fillerR++;
		cout << std::left << std::setfill(' ') << std::setw(3) << (int)*getLED(c);
		if (not (c % 3)) {
			cout << "   ";
			uint8_t c2 = c - 2;
			for (; c2 < c + 1; ++c2)
				cout << std::left << std::setfill(' ') << std::setw(3) << (int)*getLED(c2 + half);
			cout << endl;
		}
	}
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

