/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Element.cpp
 * @since     Jun 22, 2018
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2018 - 2024 Patricio A. Rossi (MeduZa)
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

#include "Element.hpp"

using namespace LEDSpicer::Devices;

Element::Element(
	const string& name,
	uint8_t* pin,
	const Color& defaultColor,
	uint timeOn,
	uint8_t brightness
) :
	name(name),
	pins{pin},
	defaultColor(defaultColor),
	timeOn(timeOn),
	// Only calculate brightness of not 0 or 100.
	brightness(brightness)
{
	pins.shrink_to_fit();
}

Element::Element(
	const string& name,
	uint8_t* pinR,
	uint8_t* pinG,
	uint8_t* pinB,
	const Color& defaultColor,
	uint8_t brightness
) :
	name(name),
	pins{pinR, pinG, pinB},
	defaultColor(defaultColor),
	// Only calculate brightness of not 0 or 100.
	brightness(brightness)
{
	pins.shrink_to_fit();
}

Element::Element(Element* other) : name(other->name), defaultColor(other->defaultColor), brightness(other->brightness) {
	for (auto p : other->pins)
		pins.push_back(nullptr);
	pins.shrink_to_fit();
}

void Element::setColor(const Color& color) {
	Color newColor = brightness ? color.fade(brightness) : color;
	if (pins.size() == 3) {
		*pins[Color::Channels::Red]   = newColor.getR();
		*pins[Color::Channels::Green] = newColor.getG();
		*pins[Color::Channels::Blue]  = newColor.getB();
	}
	else if (timeOn) {
		if (newColor == Color::Off) {
			*pins[SINGLE_PIN] = 0;
		}
		else if (not *pins[SINGLE_PIN]) {
			*pins[SINGLE_PIN] = 255;
			clockTime = std::chrono::system_clock::now() + std::chrono::milliseconds(timeOn);
		}
	}
	else {
		*pins[SINGLE_PIN] = newColor.getMonochrome();
	}
}

void Element::setColor(const Color& color, const Color::Filters& filter, uint8_t percent) {
	Color newColor = brightness ? color.fade(brightness) : color;
	if (filter == Color::Filters::Normal)
		setColor(color);
	else
		setColor(*getColor().set(color, filter, percent));
}

LEDSpicer::Color Element::getColor() {
	Color color;
	if (pins.size() == 1)
		color.set(*pins[SINGLE_PIN], *pins[SINGLE_PIN], *pins[SINGLE_PIN]);
	else
		color.set(
			*pins[Color::Channels::Red],
			*pins[Color::Channels::Green],
			*pins[Color::Channels::Blue]
		);
	return color;
}

void Element::setPinValue(uint8_t pinNumber, uint8_t val) {
	*pins[pinNumber] = val;
}

void Element::linkPin(uint8_t* val) {
	pins.push_back(val);
}

uint8_t Element::getPinValue(uint8_t pinNumber) const {
	return *pins[pinNumber];
}

uint8_t* const Element::getPin(uint8_t pinNumber) const {
	if (pinNumber > pins.size())
		throw Error("Invalid pin number");
	return pins[pinNumber];
}

const vector<uint8_t*>& Element::getPins() const {
	return pins;
}

uint8_t Element::size() const {
	return pins.size();
}

string Element::getName() {
	return name;
}

const LEDSpicer::Color& Element::getDefaultColor() {
	return defaultColor;
}

bool Element::isTimed() {
	return timeOn != 0;
}

void Element::checkTime() {
	if (*pins[SINGLE_PIN] and std::chrono::system_clock::now() > clockTime)
		*pins[SINGLE_PIN] = 0;
}

void Element::draw() {
	cout << std::left << std::setfill(' ') << std::setw(15) << name <<
		" Pin" << (pins.size() == 1 ? " " : "s") <<  ": ";
	for (auto pin : pins) {
		cout << std::setw(2) << to_string(*pin) << " ";
	}
	cout << (timeOn ? "*Timed (" + to_string(timeOn) + "ms)" : "") <<endl;
}
