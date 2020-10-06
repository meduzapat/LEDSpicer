/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Profile.cpp
 * @since     Jun 25, 2018
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

#include "Profile.hpp"

using namespace LEDSpicer::Devices;

void Profile::addAnimation(const vector<Actor*>& animation) {
	animations.insert(animations.begin(), animation.begin(), animation.end());
}

void Profile::drawConfig() {

	cout << "* Background color: " << backgroundColor.getName() << endl;

	if (startTransitions.size()) {
		cout << endl << "* Start transitions:" << endl;
		for (auto a : startTransitions)
			a->drawConfig();
	}

	if (endTransitions.size()) {
		cout << endl << "* Ending transition:" << endl;
		for (auto a : startTransitions)
			a->drawConfig();
	}

	if (animations.size()) {
		cout << endl << "* Animation Actors:" << endl;
		uint count = 1;
		for(auto actor : animations) {
			cout << "Actor " << count++ << ":" << endl;
			actor->drawConfig();
		}
	}

	if (alwaysOnGroups.size()) {
		cout << endl << "* Groups Overwrite Color: " << endl;
		for (auto& g : alwaysOnGroups) {
			cout << g.group->getName() << " ";
			g.color->drawColor();
			cout << endl;
		}
	}

	if (alwaysOnElements.size()) {
		cout << endl << "* Elements Overwrite Color: " << endl;
		for (auto& e : alwaysOnElements) {
			cout << e.element->getName() << " ";
			e.color->drawColor();
			cout << endl;
		}
	}

	if (inputs.size()) {
		cout << endl << "* Input plugins:" << endl;
		for (Input* i : inputs)
			i->drawConfig();
		cout << SEPARATOR << endl;
	}
}

void Profile::runFrame() {

	for (Input* i : inputs)
		i->process();

	bool running = false;
	if (currentActors) {
		for (auto actor : *currentActors) {
			if (actor->isRunning()) {
				running = true;
				actor->draw();
			}
		}

		if (not running) {
			if (isStarting()) {
				reset();
			}
			else if (isTerminating())
				currentActors = nullptr;
		}
	}
}

void Profile::reset() {
	currentActors = &animations;
	restartActors();
	for (Input* i : inputs)
		i->activate();
}

void Profile::restart() {
	if (startTransitions.size()) {
		currentActors = &startTransitions;
		restartActors();
		return;
	}
	reset();
}

void Profile::terminate() {
	for (Input* i : inputs)
		i->deactivate();

	if (endTransitions.size()) {
		currentActors = &endTransitions;
		restartActors();
		return;
	}
	currentActors = nullptr;
}

void Profile::restartActors() {
	for (auto actor : *currentActors)
		actor->restart();
}

bool Profile::isTransiting() const {
	return  (currentActors != &animations and (currentActors == &startTransitions or currentActors == &endTransitions));
}

bool Profile::isTerminating() const {
	return (currentActors == &endTransitions);
}

bool Profile::isStarting() const {
	return (currentActors == &startTransitions);
}

bool Profile::isRunning() const {
	return (currentActors != nullptr);
}

const LEDSpicer::Color& Profile::getBackgroundColor() const {
	return backgroundColor;
}

uint8_t Profile::getAnimationsCount() const {
	return animations.size();
}

const string& Profile::getName() const {
	return name;
}

const vector<Element::Item>& Profile::getAlwaysOnElements() const {
	return alwaysOnElements;
}

void Profile::addAlwaysOnElement(Element* element, const Color& color) {
	alwaysOnElements.emplace_back(element, &color, Color::Filters::Normal);
}

const vector<Group::Item> & Profile::getAlwaysOnGroups() const {
	return alwaysOnGroups;
}

void Profile::addAlwaysOnGroup(Group* group, const Color& color) {
	alwaysOnGroups.emplace_back(group, &color, Color::Filters::Normal);
}

void Profile::addInput(Input* input) {
	inputs.push_back(input);
}
