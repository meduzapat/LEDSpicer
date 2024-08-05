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

umap<string, Element::Item> Profile::temporaryAlwaysOnElements;
umap<string, Group::Item>   Profile::temporaryAlwaysOnGroups;

Profile::Profile(
	const string& name,
	const Color& backgroundColor,
	const vector<Actor*>& startTransitions,
	const vector<Actor*>& endTransitions,
	const milliseconds startTransitionElementsOnAt,
	const milliseconds endTransitionElementsOffAt
):
	name(name),
	backgroundColor(backgroundColor),
	startTransitions(startTransitions),
	endTransitions(endTransitions),
	currentActors(startTransitions.size() ? &this->startTransitions : &animations),
	startTransitionElementsOnAt(startTransitionElementsOnAt),
	endTransitionElementsOffAt(endTransitionElementsOffAt)
{

	float
		maxTime = 0,
		time;
	if (this->startTransitionElementsOnAt.count()) {
		// Find the most time consuming actor.
		for (auto st : this->startTransitions) {
			if (st->getRunTime() > maxTime)
				maxTime = st->getRunTime();
		}

		// Calculate the time for the elements transition.
		time = maxTime - (this->startTransitionElementsOnAt.count() / 1000.0f);
		if (time < 1)
			time = maxTime / 2;
		// Calculate the step progress.
		startStepProgress = 100.0f / (time * Actor::getFPS());
	}

	maxTime = 0;
	if (this->endTransitionElementsOffAt.count()) {
		for (auto st : this->endTransitions) {
			if (st->getRunTime() > maxTime)
				maxTime = st->getRunTime();
		}

		// Calculate the time for the elements transition.
		time = this->endTransitionElementsOffAt.count() / 1000.0f;
		if (time > maxTime)
			time = maxTime;
		// Calculate the step progress.
		endStepProgress = 100.0f / (time * Actor::getFPS());
	}
}

Profile::~Profile() {
	stop();
}

void Profile::addAnimation(const vector<Actor*>& animation) {
	animations.insert(animations.begin(), animation.begin(), animation.end());
}

void Profile::drawConfig() {

	cout << "* Background color: " << backgroundColor.getName() << endl;

	if (startTransitions.size()) {
		cout << endl << "* Start transitions:" << endl;
		if (startTransitionElementsOnAt.count())
			cout << "Show elements after: " << startTransitionElementsOnAt.count() << "ms" << endl;
		for (auto a : startTransitions)
			a->drawConfig();
	}


	if (endTransitions.size()) {
		cout << endl << "* Ending transition:" << endl;
		if (endTransitionElementsOffAt.count())
			cout << "Hide elements after: " << endTransitionElementsOffAt.count() << "ms" << endl;
		for (auto a : endTransitions)
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
	// Keeps track of the currect batch of actors (start, normal, or ending).
	bool running = false;

	if (currentActors) {
		for (auto actor : *currentActors) {
			// If the actor is running so draw it.
			if (actor->isRunning()) {
				running = true;
				actor->draw();
			}
		}

		// The current animation set finished.
		if (not running) {
			// Starting animation ended.
			if (isStarting()) {
				if (transitionEndTime) {
					delete transitionEndTime;
					transitionEndTime = nullptr;
					elementProgress = 100;
				}
				reset();
			}
			else if (isTerminating()) {
				// Ending animation ended.
				if (transitionEndTime) {
					delete transitionEndTime;
					transitionEndTime = nullptr;
					elementProgress = 0;
				}
				currentActors = nullptr;
			}
		}
		else {
			runAlwaysOnElements(false);
		}
	}

	// Do not display elements and groups while the profile is transitioning.
	if (not isTransiting() and currentActors) {
		// Set always on groups from profile.
		for (auto& gE : alwaysOnGroups) {
			gE.process(50);
		}

		// Set always on elements from profile.
		runAlwaysOnElements(true);

		// Set always on groups from temporary requests.
		for (auto& gE : temporaryAlwaysOnGroups) {
			gE.second.process(50);
		}

		// Set always on elements from temporary requests.
		for (auto& eE : temporaryAlwaysOnElements) {
			eE.second.process(50);
		}

		// Set controlled items from input plugins.
		for (auto& item : Input::getControlledInputs()) {
			item.second->process(50);
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
		if (startTransitionElementsOnAt > std::chrono::milliseconds(0)) {
			transitionEndTime = new time_point<std::chrono::system_clock>(std::chrono::system_clock::now() + startTransitionElementsOnAt);
			elementProgress = 0;
		}
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
		if (endTransitionElementsOffAt > std::chrono::milliseconds(0)) {
			transitionEndTime = new time_point<std::chrono::system_clock>(std::chrono::system_clock::now() + endTransitionElementsOffAt);
			elementProgress = 100;
		}
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
	if ((Utility::globalFlags & FLAG_NO_START_TRANSITIONS) and (Utility::globalFlags & FLAG_NO_END_TRANSITIONS)) {
		return false;
	}
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

void Profile::addAlwaysOnElement(Element* element, const Color& color) {
	alwaysOnElements.emplace_back(element, &color, Color::Filters::Normal);
}

void Profile::addAlwaysOnGroup(Group* group, const Color& color) {
	alwaysOnGroups.emplace_back(group, &color, Color::Filters::Normal);
}

void Profile::addTemporaryAlwaysOnElement(const string name, const Element::Item item) {
	temporaryAlwaysOnElements.emplace(name, item);
}

void Profile::removeTemporaryAlwaysOnElement(const string name) {
	if (temporaryAlwaysOnElements.count(name))
			temporaryAlwaysOnElements.erase(name);
}

void Profile::removeTemporaryAlwaysOnElements() {
	temporaryAlwaysOnElements.clear();
}

void Profile::addTemporaryAlwaysOnGroup(const string name, const Group::Item item) {
	temporaryAlwaysOnGroups.emplace(name, item);
}

void Profile::removeTemporaryAlwaysOnGroup(const string name) {
	if (temporaryAlwaysOnGroups.count(name))
			temporaryAlwaysOnGroups.erase(name);
}

void Profile::removeTemporaryAlwaysOnGroups() {
	temporaryAlwaysOnGroups.clear();
}

void Profile::addInput(Input* input) {
	inputs.push_back(input);
}

void Profile::runAlwaysOnElements(bool force) {
	Color::Filters filter(Color::Filters::Normal);
	if (not force) {
		if (not transitionEndTime)
			return;
		if (isStarting()) {
			if (std::chrono::system_clock::now() < *transitionEndTime)
				return;
			elementProgress += startStepProgress;
			elementProgress = elementProgress > 100 ? 100 : elementProgress;
			LogDebug(to_string(elementProgress) + " / 100");
		}
		if (isTerminating()) {
			if (std::chrono::system_clock::now() >= *transitionEndTime)
				return;
			elementProgress -= endStepProgress;
			elementProgress = elementProgress < 0 ? 0 : elementProgress;
			LogDebug(to_string(elementProgress) + " / 0");
		}
		filter = Color::Filters::Combine;
	}

	// Set always on elements from profile.
	for (auto& eE : alwaysOnElements) {
		// Ignore ways if the flag is set.
		if ((Utility::globalFlags & FLAG_NO_ROTATOR) and eE.element->getName().find(WAYS_INDICATOR) != string::npos)
			continue;
		eE.filter = filter;
		eE.process(force ? 50 : elementProgress);
	}
}
