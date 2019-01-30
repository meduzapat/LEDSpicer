/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Board.cpp
 * @ingroup
 * @since     Jun 7, 2018
 * @author    Patricio A. Rossi (MeduZa)
 * @copyright Copyright © 2018 - 2019 Patricio A. Rossi (MeduZa)
 */

#include "Device.hpp"

using namespace LEDSpicer::Devices;

void Device::afterClaimInterface() {
	LogDebug("Initializing board");
	setLeds(0);
	transfer();
}

Device* Device::setLed(uint8_t led, uint8_t intensity) {
	validateLed(led);
	LEDs[led] = intensity;
	return this;
}

Device* Device::setLeds(uint8_t intensity) {
	for (auto& led : LEDs)
		led = intensity;
	return this;
}

uint8_t* Device::getLed(uint8_t ledPos) {
	validateLed(ledPos);
	return &LEDs.at(ledPos);
}

void Device::registerElement(const string& name, uint8_t led) {
	validateLed(led);
	elementsByName.emplace(name, Element(name, &LEDs[led]));
}

void Device::registerElement(const string& name, uint8_t led1, uint8_t led2, uint8_t led3) {
	validateLed(led1);
	validateLed(led2);
	validateLed(led3);
	elementsByName.emplace(name, Element(name, &LEDs[led1], &LEDs[led2], &LEDs[led3]));
}

Element* Device::getElement(const string& name) {
	return &elementsByName.at(name);
}

void Device::validateLed(uint8_t led) const {
	if (led >= LEDs.size())
		throw Error("Invalid led number " + to_string(led));
}

uint8_t Device::getNumberOfElements() const {
	return elementsByName.size();
}

umap<string, Element>* Device::getElements() {
	return &elementsByName;
}
