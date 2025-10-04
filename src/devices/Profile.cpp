/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Profile.cpp
 * @since     Jun 25, 2018
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

#include "Profile.hpp"

using namespace LEDSpicer::Devices;

Profile* Profile::defaultProfile = nullptr;
ElementItemUMap Profile::temporaryOnElements;
GroupItemUMap   Profile::temporaryOnGroups;

void Profile::addAnimation(const vector<Actor*>& animation) {
	animations.insert(animations.begin(), animation.begin(), animation.end());
}

void Profile::drawConfig() const {

	cout << "* Background color: " << backgroundColor.getName() << endl;

	if (alwaysOnGroups.size()) {
		cout << endl << "* Groups Overwrite Color: " << endl;
		for (auto& g : alwaysOnGroups) {
			cout << g.group->getName() << " " << g.color->getName() << endl;
		}
	}

	if (alwaysOnElements.size()) {
		cout << endl << "* Elements Overwrite Color: " << endl;
		for (auto& e : alwaysOnElements) {
			cout << e.element->getName() << " " << e.color->getName() << endl;
		}
	}

	if (animations.size()) {
		cout << endl << "* Animation Actors:" << endl;
		uint count = 1;
		for(auto actor : animations) {
			cout << "Actor " << count++ << ":" << endl;
			actor->drawConfig();
		}
	}

	if (inputs.size()) {
		cout << endl << "* Input plugins:" << endl;
		for (Input* i : inputs) i->drawConfig();
		cout << SEPARATOR << endl;
	}
}

void Profile::runFrame(bool advanceFrame) {

	if (advanceFrame) Actor::newFrame();

	// Reset elements.
	for (const auto& e : Element::allElements)
		if (e.second->isTimed())
			e.second->checkTime();
		else
			e.second->setColor(backgroundColor);

	if (not (Utility::globalFlags & FLAG_NO_INPUTS)) {
		for (Input* i : inputs) i->process();
	}

	if (not (Utility::globalFlags & FLAG_NO_ANIMATIONS)) {
		// Draw current animations if any.
		for (auto actor : animations) actor->draw();
	}

	// Set always on groups from profile.
	for (auto& gE : alwaysOnGroups) {
		gE.process(50, nullptr);
	}

	// Set always on elements from profile.
	for (auto& eE : alwaysOnElements) {
		eE.process(50, nullptr);
	}

	// Set always on groups from temporary requests.
	for (auto& gE : temporaryOnGroups) {
		gE.second.process(50, nullptr);
	}

	// Set always on elements from temporary requests.
	for (auto& eE : temporaryOnElements) {
		eE.second.process(50, nullptr);
	}

	// Set controlled items from input plugins.
	for (auto& item : Input::getControlledInputs()) {
		item.second->process(50, nullptr);
	}
}

void Profile::reset() {
	if (not (Utility::globalFlags & FLAG_NO_ANIMATIONS))
		for (auto actor : animations) actor->restart();
	if (not (Utility::globalFlags & FLAG_NO_INPUTS)) startInputs();
	Profile::removeTemporaryOnGroups();
	Profile::removeTemporaryOnElements();
	Input::clearControlledInputs();
}

void Profile::startInputs() {
	for (Input* i : inputs) i->activate();
}

void Profile::stopInputs() {
	for (Input* i : inputs) i->deactivate();
}

const Color& Profile::getBackgroundColor() const {
	return backgroundColor;
}

const string& Profile::getName() const {
	return name;
}

void Profile::addAlwaysOnElement(Element* element, const Color& color, const Color::Filters& filter) {
	alwaysOnElements.emplace_back(element, &color, filter);
}

void Profile::addAlwaysOnGroup(Group* group, const Color& color, const Color::Filters& filter) {
	alwaysOnGroups.emplace_back(group, &color,filter);
}

void Profile::addTemporaryOnElement(const string name, const Element::Item item) {
	removeTemporaryOnElement(name);
	temporaryOnElements.emplace(name, item);
}

void Profile::removeTemporaryOnElement(const string name) {
	if (temporaryOnElements.exists(name)) temporaryOnElements.erase(name);
}

void Profile::removeTemporaryOnElements() {
	temporaryOnElements.clear();
}

void Profile::addTemporaryOnGroup(const string name, const Group::Item item) {
	removeTemporaryOnGroup(name);
	temporaryOnGroups.emplace(name, item);
}

void Profile::removeTemporaryOnGroup(const string name) {
	if (temporaryOnGroups.exists(name)) temporaryOnGroups.erase(name);
}

void Profile::removeTemporaryOnGroups() {
	temporaryOnGroups.clear();
}

void Profile::addInput(Input* input) {
	inputs.push_back(input);
}

vector<uint8_t*> Profile::collectUniqueLeds() const {
	vector<uint8_t*> currentLeds;
	unordered_set<uint8_t*> seen;

	for (const auto actor : animations) {
		const auto& leds = actor->getLeds();
		for (auto* led : leds) {
			if (seen.insert(led).second) { // true if newly inserted
				currentLeds.push_back(led);
			}
		}
	}
	return currentLeds;
}
