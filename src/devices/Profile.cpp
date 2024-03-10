/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Profile.cpp
 * @since     Jun 25, 2018
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
		for (Input* i : inputs)
			i->drawConfig();
		cout << SEPARATOR << endl;
	}
}

void Profile::runFrame() {

	Actor::newFrame();

	if (not (Utility::globalFlags & FLAG_NO_INPUTS)) {
		for (Input* i : inputs)
			i->process();
	}
	// To differentiate between transitions and normal animation..
	bool running = false;

	if (currentActors) {
		for (auto actor : *currentActors) {
			// If the actor is running draw.
			if (actor->isRunning()) {
				running = true;
				actor->draw();
			}
		}

		// The current animation set finished.
		if (not running) {
			// Stating animation ended.
			if (isStarting()) {
				reset();
			}
			else if (isTerminating()) {
				// Ending animation ended.
				currentActors = nullptr;
			}
		}
	}
}

void Profile::reset() {
	if (not (Utility::globalFlags & FLAG_NO_ANIMATIONS)) {
		LogDebug("Running animations");
		currentActors = &animations;
		restartActors();
	}
	if (not (Utility::globalFlags & FLAG_NO_INPUTS)) {
		for (Input* i : inputs)
			i->activate();
	}
}

void Profile::restart() {
	if (startTransitions.size() and not (Utility::globalFlags & FLAG_NO_START_TRANSITIONS)) {
		LogDebug("Running starting transitions");
		currentActors = &startTransitions;
		restartActors();
		return;
	}
	reset();
}

void Profile::stop() {
	for (Input* i : inputs)
		i->deactivate();
}

void Profile::terminate() {
	stop();
	if (endTransitions.size() and not (Utility::globalFlags & FLAG_NO_END_TRANSITIONS)) {
		LogDebug("Running ending transitions");
		currentActors = &endTransitions;
		restartActors();
		return;
	}
	// This will provoke that the current profile get replaced.
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
