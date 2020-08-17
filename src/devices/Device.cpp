/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Board.cpp
 *
 * @since     Jun 7, 2018
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

#include "Device.hpp"

using namespace LEDSpicer::Devices;

Device::Device(
	uint8_t  elements,
	const string& name
) : name(name) {
	LEDs.resize(elements);
	oldLEDs.resize(elements, 1);
	LEDs.shrink_to_fit();
	oldLEDs.shrink_to_fit();
}

void Device::initialize() {
	LogDebug("Initializing Device " + name);
	openDevice();
	resetLeds();
}

void Device::terminate() {
	LogDebug("Disconnect Device " + name);
	resetLeds();
	closeDevice();
}

Device* Device::setLed(uint8_t led, uint8_t intensity) {
#ifdef DEVELOP
	validateLed(led);
#endif
	LEDs[led] = intensity;
	return this;
}

Device* Device::setLeds(uint8_t intensity) {
	for (auto& led : LEDs)
		led = intensity;
	return this;
}

uint8_t* Device::getLed(uint8_t ledPos) {
#ifdef DEVELOP
	validateLed(ledPos);
#endif
	return &LEDs.at(ledPos);
}

void Device::registerElement(const string& name, uint8_t led, const Color& defaultColor) {
	validateLed(led);
	elementsByName.emplace(name, Element(name, &LEDs[led], defaultColor));
}

void Device::registerElement(
	const string& name,
	uint8_t led1,
	uint8_t led2,
	uint8_t led3,
	const Color& defaultColor
) {
	validateLed(led1);
	validateLed(led2);
	validateLed(led3);
	elementsByName.emplace(name, Element(name, &LEDs[led1], &LEDs[led2], &LEDs[led3], defaultColor));
}

Element* Device::getElement(const string& name) {
	return &elementsByName.at(name);
}

void Device::validateLed(uint8_t led) const {
	if (led >= LEDs.size())
		throw Error("Invalid led number " + to_string(led + 1));
}

uint8_t Device::getNumberOfElements() const {
	return elementsByName.size();
}

uint8_t Device::getNumberOfLeds() {
	return LEDs.size();
}

void Device::packData() {
	if (LEDs == oldLEDs) {
#ifdef DEVELOP
	#ifdef SHOW_OUTPUT
	LogDebug("No changes, data not sent for " + getFullName());
	#endif
#endif
		return;
	}
	transfer();
	oldLEDs = LEDs;
}

umap<string, Element>* Device::getElements() {
	return &elementsByName;
}
