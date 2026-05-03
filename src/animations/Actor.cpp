/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Actor.cpp
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

#include "Actor.hpp"

using namespace LEDSpicer::Animations;

uint8_t Actor::FPS   = 0;
uint8_t Actor::frame = 0;

Actor::Actor(
	StringUMap& parameters,
	Group* const group,
	const vector<string>& requiredParameters
) :
	filter(Color::str2filter(parameters.exists("filter") ? parameters["filter"] : "Normal")),
	secondsToStart(parameters.exists("startTime")     ? Utility::parseNumber(parameters["startTime"],   "Invalid Value for start time")   : 0),
	secondsToEnd(parameters.exists("endTime")         ? Utility::parseNumber(parameters["endTime"],     "Invalid Value for end time")     : 0),
	secondsToRestart(parameters.exists("restartTime") ? Utility::parseNumber(parameters["restartTime"], "Invalid Value for restart time") : 0),
	affectedElements(group->size(), false),
	group(group),
	repeat(parameters.exists("repeat") ? Utility::parseNumber(parameters["repeat"], "Invalid Value for repeat") : 0)
{
	affectedElements.shrink_to_fit();
	Utility::checkAttributes(requiredParameters, parameters, "actor");
	if (secondsToRestart and not secondsToEnd)
		LogWarning("restartTime has no effect without endTime");
}

Group& Actor::getGroup() const {
	return *group;
}

void Actor::draw() {

	wasRunning = isRunning();
	if (not wasRunning) return;

	// Reset Affected.
	affectAllElements();
	calculateElements();
	// After effects.
	if (not affectedElements.empty() and filter == Color::Filters::Mask) {
		// turn off any non affected element.
		for (uint16_t elIdx = 0; elIdx < group->size(); ++elIdx) {
			if (affectedElements[elIdx]) continue;
			changeElementColor(elIdx, Color::Off, Color::Filters::Normal, 100);
		}
	}
}

void Actor::drawConfig() const {

	cout <<
		"Group: "  << group->getName()          << endl <<
		"Filter: " << Color::filter2str(filter) << endl;

	if (secondsToStart)
		cout << "Start After: "   << secondsToStart   << " sec"   << endl;
	if (secondsToEnd)
		cout << "Stop After: "    << secondsToEnd     << " sec"   << endl;
	if (secondsToRestart)
		cout << "Restart After: " << secondsToRestart << " sec"   << endl;
	if (repeat)
		cout << "Will repeat: "   << repeat           << " times" << endl;

	auto rt {getRunTime()};
	cout << "Total running time: " << (rt > 0.0f ? to_string(rt) : "∞")
		 << " sec"    << endl
		 << SEPARATOR << endl;
}

void Actor::restart() {
#ifdef DEVELOP
	LogDebug("Actor restarting");
#endif
	if (endTime) {
		delete endTime;
		endTime = nullptr;
	}

	if (restartTime) {
		delete restartTime;
		restartTime = nullptr;
	}

	if (secondsToStart) {
		if (startTime) delete startTime;
		startTime = new Time(secondsToStart);
	}
	else if (secondsToEnd) {
		endTime = new Time(secondsToEnd);
	}
}

bool Actor::isRunning() {

	if (startTime) {
		// Start time is running (actor off).
		if (not startTime->isTime()) return false;
		// Start time is over (actor on)
		delete startTime;
		startTime = nullptr;
#ifdef DEVELOP
		LogDebug("Starting Actor by time after " + to_string(secondsToStart) + " seconds");
#endif
		// Set end clock.
		if (secondsToEnd) endTime = new Time(secondsToEnd);
	}

	if (endTime and endTime->isTime()) {
		// End time (actor off).
		delete endTime;
		endTime = nullptr;
#ifdef DEVELOP
		LogDebug("Ended Actor by time after " + to_string(secondsToEnd) + " seconds");
#endif
		// Set restart clock if any.
		if (secondsToRestart) {
			restartTime = new Time(secondsToRestart);
		}
		return false;
	}

	if (restartTime) {
		// Restart clock running.
		if (not restartTime->isTime()) return false;
		// Restart time reached, check repeats.
		delete restartTime;
		restartTime = nullptr;
	#ifdef DEVELOP
		LogDebug("Restarting Actor after " + to_string(secondsToRestart) + " seconds");
	#endif
		checkRepeats();
		return false;
	}

	if (not endTime and secondsToEnd) {
		checkRepeats();
		return endTime;
	}
	return true;
}

void Actor::setFPS(uint8_t FPS) {
	LogDebug("Setting FPS to " + to_string(FPS));
	Actor::FPS = FPS;
}

uint8_t Actor::getFPS() {
	return FPS;
}

void Actor::newFrame() {
	if (frame == FPS) {
		frame = 0;
		Time::setFrameTime();
	}
	++frame;
}

uint16_t Actor::getNumberOfElements() const {
	return group->getElements().size();
}

void Actor::changeElementColor(uint16_t index, const Color& color, Color::Filters filter, uint8_t percent) {
	affectedElements[index] = true;
	Element* e = group->getElement(index);
	e->setColor(color, filter, percent);
}

void Actor::changeElementsColor(const Color& color, Color::Filters filter, uint8_t percent) {
	for (uint16_t c = 0; c < group->size(); ++c)
		changeElementColor(c, color, filter, percent);
}

const vector<uint8_t*>& Actor::getLeds() const {
	return group->getLeds();
}

void Actor::affectAllElements(bool value) {
	affectedElements.assign(group->size(), value);
}

bool Actor::isElementAffected(uint16_t index) const {
	return affectedElements[index];
}

bool Actor::checkRepeats() {
	if (repeat == 1 or (repeat and repeated == repeat)) return false;
	if (repeat) ++repeated;
	restart();
	return true;
}
