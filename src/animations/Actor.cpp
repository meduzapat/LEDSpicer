/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Actor.cpp
 * @since     Jun 22, 2018
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

#include "Actor.hpp"

using namespace LEDSpicer::Animations;

uint8_t Actor::FPS = 0;

Actor::Actor(
	umap<string, string>& parameters,
	Group* const group,
	const vector<string>& requiredParameters
) :
	filter(Color::str2filter(parameters.count("filter") ? parameters["filter"] : "Normal")),
	secondsToStart(parameters.count("startTime") ? Utility::parseNumber(parameters["startTime"], "Invalid Value for start time") : 0),
	secondsToEnd(parameters.count("endTime") ? Utility::parseNumber(parameters["endTime"], "Invalid Value for end time") : 0),
	group(group)
{
	affectedElements.resize(group->size(), false);
	affectedElements.shrink_to_fit();
	Utility::checkAttributes(requiredParameters, parameters, "actor.");
}

void Actor::draw() {

	affectAllElements();
	calculateElements();
	if (not affectedElements.empty())
		switch (filter) {
		case Color::Filters::Mask: {
			const Color& black = Color::getColor("Black");
			// turn off any non affected element.
			for (uint8_t elIdx = 0; elIdx < group->size(); ++elIdx) {
				if (affectedElements[elIdx])
					continue;
				changeElementColor(elIdx, black, Color::Filters::Normal, 100);
			}
		}
	}
}

void Actor::drawConfig() {
	cout <<
		"Group: " << group->getName() << endl <<
		"Filter: " << Color::filter2str(filter) << endl;
	if (secondsToStart)
		cout << "Start After: " << secondsToStart << " sec" << endl;
	if (secondsToEnd)
		cout << "Stop After: " << secondsToEnd << " sec" << endl;
}

void Actor::restart() {
#ifdef DEVELOP
	LogDebug("Actor restarting");
#endif
	if (endTime) {
		delete endTime;
		endTime = nullptr;
	}

	if (secondsToStart) {
		if (startTime)
			delete startTime;
		startTime = new Time(secondsToStart);
	}
	else if (secondsToEnd) {
		endTime = new Time(secondsToEnd);
	}
}

bool Actor::isRunning() {

	if (startTime) {
		if (not startTime->isTime())
			return false;
		delete startTime;
		startTime = nullptr;
#ifdef DEVELOP
		LogDebug("Staring Actor by time after " + to_string(secondsToStart) + " seconds.");
#endif
		if (secondsToEnd)
			endTime = new Time(secondsToEnd);
	}

	if (endTime and endTime->isTime()) {
		delete endTime;
		endTime = nullptr;
#ifdef DEVELOP
		LogDebug("Ended Actor by time after " + to_string(secondsToEnd) + " seconds.");
#endif
	}

	// Time Over: there is an end time that ran out, and not start time, so is not running.
	if (not startTime and not endTime and secondsToEnd)
		return false;

	return true;
}

void Actor::setFPS(uint8_t FPS) {
	LogDebug("Setting FPS to " + to_string(FPS));
	Actor::FPS = FPS;
}

uint8_t Actor::getFPS() {
	return FPS;
}

void Actor::setStartTime(uint16_t seconds) {
	secondsToStart = seconds;
}

void Actor::setEndTime(uint16_t seconds) {
	secondsToEnd = seconds;
}

uint8_t Actor::getNumberOfElements() const {
	return group->getElements().size();
}

void Actor::changeElementColor(uint8_t index, const Color& color, Color::Filters filter, uint8_t percent) {
	affectedElements[index] = true;
	Element* e = group->getElement(index);
	e->setColor(color, filter, percent);
}

void Actor::changeElementsColor(const Color& color, Color::Filters filter, uint8_t percent) {
	for (uint8_t c = 0; c < group->size(); ++c)
		changeElementColor(c, color, filter, percent);
}

void Actor::affectAllElements(bool value) {
	affectedElements.assign(group->size(), value);
}

bool Actor::isElementAffected(uint8_t index) {
	return affectedElements[index];
}
