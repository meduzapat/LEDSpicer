/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Board.cpp
 *
 * @since     Jun 7, 2018
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2018 - 2025 Patricio A. Rossi (MeduZa)
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

void Device::initialize() {

	if (dumpMode)
		return;

	LogDebug("Initializing Device " + getFullName());
	openHardware();
	resetLeds();
}

void Device::terminate() {

	if (dumpMode)
		return;

	LogDebug("Disconnect Device " + getFullName());
	resetLeds();
	closeHardware();
}

Device* Device::setLed(uint16_t led, uint8_t intensity) {
#ifdef DEVELOP
	validateLed(led);
#endif
	LEDs[led] = intensity;
	return this;
}

Device* Device::setLeds(uint8_t intensity) {
	std::fill(LEDs.begin(), LEDs.end(), intensity);
	return this;
}

uint8_t* Device::getLed(uint16_t ledPos) {
#ifdef DEVELOP
	validateLed(ledPos);
#endif
	return &LEDs.at(ledPos);
}

void Device::registerElement(
	const string& name,
	uint16_t led,
	const Color& defaultColor,
	uint timeOn,
	uint8_t brightness
) {
	validateLed(led);
	elementsByName.emplace(name, Element(name, &LEDs[led], defaultColor, timeOn, brightness));
}

void Device::registerElement(
	const string& name,
	uint16_t led1,
	uint16_t led2,
	uint16_t led3,
	const Color& defaultColor,
	uint8_t brightness
) {
	validateLed(led1);
	validateLed(led2);
	validateLed(led3);
	elementsByName.emplace(name, Element(
		name,
		&LEDs[led1], &LEDs[led2], &LEDs[led3],
		defaultColor,
		brightness
	));
}

void Device::registerElement(
	const string& name,
	const vector<uint16_t>& ledPositions,
	const Color& defaultColor,
	uint8_t brightness
) {
	vector<uint8_t*> leds;
	for (uint16_t led : ledPositions) {
		validateLed(led);
		// Map the LED positions to pointers
		leds.push_back(&LEDs[led]);
	}

	elementsByName.emplace(name, Element(
		name,
		leds,
		defaultColor,
		brightness
	));
}

Element* Device::getElement(const string& name) {
	return &elementsByName.at(name);
}

void Device::resetLeds() {
	setLeds(0);
	transfer();
}

void Device::validateLed(uint16_t led) const {
	if (led >= LEDs.size())
		throw Error("Invalid led number " + to_string(led + 1));
}

uint16_t Device::getNumberOfElements() const {
	return elementsByName.size();
}

uint16_t Device::getNumberOfLeds() const {
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

