/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Group.cpp
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

#include "Group.hpp"

using namespace LEDSpicer::Devices;

unordered_map<string, Group> Group::layout;

void Group::drawElements() {
	cout << elements.size() << " Element(s): " << endl;
	for (auto element : elements)
		cout << element->getName() << endl;
}

uint16_t Group::size() const {
	return elements.size();
}

void Group::linkElement(Element* element) {
	elements.push_back(element);
	// Append this element’s LEDs to cached list if not solenoid.
	if (element->isTimed())
		return;
	const auto& eLeds = element->getLeds();
	leds.insert(leds.end(), eLeds.begin(), eLeds.end());
}

void Group::shrinkToFit() {
	elements.shrink_to_fit();
	leds.shrink_to_fit();
}

const vector<Element*>& Group::getElements() const {
	return elements;
}

Element* Group::getElement(uint16_t index) const {
	return elements.at(index);
}

const string& Group::getName() const {
	return name;
}

const Color& Group::getDefaultColor() const {
	return defaultColor;
}

const vector<uint8_t*>& Group::getLeds() const {
	return leds;
}
