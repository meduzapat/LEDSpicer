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

Actor::Actor(
	StringUMap& parameters,
	Group* const group,
	const vector<string>& requiredParameters
) :
	actorNumber{actorCount++},
	filter(Color::str2filter(parameters.exists("filter") ? parameters["filter"] : "Normal")),
	secondsToStart(parameters.exists("startTime")     ? Utility::parseNumber(parameters["startTime"],   "Invalid Value for start time")   : 0),
	secondsToEnd(parameters.exists("endTime")         ? Utility::parseNumber(parameters["endTime"],     "Invalid Value for end time")     : 0),
	secondsToRestart(parameters.exists("restartTime") ? Utility::parseNumber(parameters["restartTime"], "Invalid Value for restart time") : 0),
	// -1 = repeat forever, 0 never repeat, > 0 repeat times.
	repeat(parameters.exists("repeat") ? Utility::parseNumber(parameters["repeat"], "Invalid Value for repeat") : 0),
	affectedElements(group->size(), false),
	group(group)
{
	affectedElements.shrink_to_fit();
	Utility::checkAttributes(requiredParameters, parameters, "actor");
	if (secondsToRestart) {
		if (not secondsToEnd) {
			LogWarning("restartTime has no effect without endTime");
		}
		if (repeat < 0) {
			LogWarning("Always repeat is set, restartTime has no effect");
		}
	}
	if (not secondsToEnd) {
		if (repeat < 0) {
			LogWarning("Repeat has no effect when endTime is not set");
		}
	}
#ifdef DEVELOP
	LogDebug("Actor " + std::to_string(actorNumber) + " Initiated");
#endif
}

Actor::~Actor() {
	if (startTime) {
		delete startTime;
		startTime = nullptr;
	}
	if (endTime) {
		delete endTime;
		endTime = nullptr;
	}
	if (restartTime) {
		delete restartTime;
		restartTime = nullptr;
	}
}

Group& Actor::getGroup() const {
	return *group;
}

void Actor::draw() {

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
		"Actor ID: " << actorNumber               << endl <<
		"Group: "    << group->getName()          << endl <<
		"Filter: "   << Color::filter2str(filter) << endl;

	auto rt {getRunTime()};
	cout << "Running time:   ";
	if (rt > 0.0f)
		cout << std::fixed << std::setprecision(2) << rt << " sec";
	else
		cout << "∞";
	cout << endl;

	if (secondsToStart)
		cout << "Start After:    " << std::fixed << std::setprecision(2) << secondsToStart   << " sec" << endl;
	if (secondsToEnd)
		cout << "Stop After:     " << std::fixed << std::setprecision(2) << secondsToEnd     << " sec" << endl;
	if (repeat >= 0 and secondsToRestart)
		cout << "Restart After:  " << std::fixed << std::setprecision(2) << secondsToRestart << " sec" << endl;
	if (repeat)
		cout << "Will repeat:    " << (repeat > 0 ? std::to_string(repeat) : "∞") << " times" << endl;
}

void Actor::restart() {
#ifdef DEVELOP
	LogDebug("Actor " + std::to_string(actorNumber) + " restarting");
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

	// Note: Return false after restart, so startTime avoids artifacts.

#ifdef DEVELOP
	if (frame == 1)
		cout
			<< "Actor: " + to_string(actorNumber) << " "
			<< "ST " << (secondsToStart   ? "+" : "-") << "/" << (startTime   ? "+" : "-") << "  "
			<< "ET " << (secondsToEnd     ? "+" : "-") << "/" << (endTime     ? "+" : "-") << "  "
			<< "RT " << (secondsToRestart ? "+" : "-") << "/" << (restartTime ? "+" : "-") << "  "
			<< "R "  << (repeat < 0 ? "-/∞" : repeat == 0 ? "-/-" : to_string(repeated) + "/" + to_string(repeat)) << "  "
			<< (not startTime and not restartTime ? (not endTime and secondsToEnd ? "OFF" : "ON") : "OFF")
			<< endl;
#endif

	const auto resetTimer = [](Time*& t) noexcept { delete t; t = nullptr; };

	if (startTime) {
		// Start time is running (actor off).
		if (not startTime->isTime()) return false;

		// Start time is over (actor goes on)
		resetTimer(startTime);
#ifdef DEVELOP
		LogDebug("Starting Actor " + std::to_string(actorNumber) + " by time after " + to_string(secondsToStart) + " seconds");
#endif
		// Arm endTime.
		if (secondsToEnd) endTime = new Time(secondsToEnd);
		return false;
	}

	if (endTime) {

		// not time to end, running.
		if (not endTime->isTime()) return true;

		// End time (actor goes off).
		resetTimer(endTime);
#ifdef DEVELOP
		LogDebug("Ended Actor " + to_string(actorNumber) + " by time after " + to_string(secondsToEnd) + " seconds");
#endif
		// -1 repeat forever after end time.
		if (repeat < 0) {
			restart();
			return false;
		}

		if (repeat and repeated < repeat) {
#ifdef DEVELOP
			LogDebug("Repeat consumed for actor " + to_string(actorNumber));
#endif
			restart();
			++repeated;
			return false;
		}

		if (secondsToRestart) {
#ifdef DEVELOP
			LogDebug("Restart timer set on actor " + to_string(actorNumber));
#endif
			restartTime = new Time(secondsToRestart);
		}
		return false;
	}

	if (restartTime) {
		if (not restartTime->isTime()) return false;
		// Restart time reached, restart actor.
		resetTimer(restartTime);

		// reset repeat.
		if (repeat > 0) repeated = 0;
#ifdef DEVELOP
		LogDebug("Restarting Actor " + std::to_string(actorNumber) + " after " + to_string(secondsToRestart) + " seconds");
#endif
		restart();
		return false;
	}

	// Flag to avoid depleted repeats go into running.
	if (not endTime and secondsToEnd) return false;

	// Normal run.
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

float Actor::getRunTime() const {
	// repeat -1 is ∞
	if (repeat < 0) return 0;
	return secondsToEnd * (repeat > 0 ? repeat + 1 : 1);
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
