/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Element.cpp
 * @since     Jun 22, 2018
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2018 - 2026 Patricio A. Rossi (MeduZa)
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

unordered_map<string, Element*> Element::allElements;

void Element::setColor(const Color& color) {
	const Color& newColor(brightness ? color.fade(brightness) : color);
	// Single RGB or Multiple RGB.
	if (leds.size() >= 3) {
		for (size_t i = 0; i < leds.size(); i += 3) {
			*leds[i + Color::Channels::Red]   = newColor.getR();
			*leds[i + Color::Channels::Green] = newColor.getG();
			*leds[i + Color::Channels::Blue]  = newColor.getB();
		}
	}
	// Handle timed motors or solenoids.
	else if (timeOn) {
		// set off
		if (newColor == Color::Off) {
			*leds[SINGLE_LED] = 0;
		}
		// On, always kick full intense.
		else if (not *leds[SINGLE_LED]) {
			*leds[SINGLE_LED] = 255;
			reset(timeOn);
		}
	}
	// Single led monochrome.
	else {
		*leds[SINGLE_LED] = newColor.getMonochrome();
	}
}

void Element::setColor(const Color& color, const Color::Filters& filter, uint8_t percent) {
	if (filter == Color::Filters::Normal)
		setColor(color);
	else
		setColor(*getColor().set(color, filter, percent));
}

Color Element::getColor() const {
	Color color;
	if (leds.size() == 1)
		color.set(*leds[SINGLE_LED], *leds[SINGLE_LED], *leds[SINGLE_LED]);
	else
		color.set(
			*leds[Color::Channels::Red],
			*leds[Color::Channels::Green],
			*leds[Color::Channels::Blue]
		);
	return color;
}

void Element::setLedValue(uint16_t led, uint8_t val) {
	*leds[led] = val;
}

uint8_t Element::getLedValue(uint16_t led) const {
	return *leds[led];
}

uint8_t* Element::getLed(uint16_t led) const {
	if (led >= leds.size()) throw Error("Invalid led number");
	return leds[led];
}

const vector<uint8_t*>& Element::getLeds() const {
	return leds;
}

uint8_t Element::size() const {
	return leds.size();
}

const string Element::getName() const {
	return name;
}

const Color& Element::getDefaultColor() const {
	return defaultColor;
}

bool Element::isTimed() const {
	return timeOn != 0;
}

void Element::checkTime() {
	// If is on and ran out of time, set it off.
	if (*leds[SINGLE_LED] and isTime()) *leds[SINGLE_LED] = 0;
}

void Element::draw() const {
	cout <<
		std::left << std::setfill(' ') << std::setw(20) << name <<
		" Led: " << (leds.size() == 1 ? (timeOn ? "Solenoid" : "Single Color") : (leds.size() == 3 ? "RGB" : "Multi RGB"));

	if (timeOn)
		cout << std::left << std::setw(6) << (to_string(timeOn) + "ms");

	cout <<
		" Default Color: " << std::left << std::setw(15) << defaultColor.getName() <<
		(brightness ? " Brightness:" + to_string(brightness) + "%" : "") << endl;

	if (timeOn)
		cout << "  * Connected timed hardware like a solenoids" << endl;
}
