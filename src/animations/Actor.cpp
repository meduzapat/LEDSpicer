/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Actor.cpp
 * @since     Jun 22, 2018
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2018 - 2019 Patricio A. Rossi (MeduZa)
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

uint8_t Actor::currentFrame = 1;
uint8_t Actor::FPS          = 0;

Actor::Actor(
	umap<string, string>& parameters,
	Group* const group,
	const vector<string>& requiredParameters
) :
	filter(Color::str2filter(parameters["filter"])),
	group(group)
{
	if (parameters.count("direction"))
		direction = str2direction(parameters["direction"]);
	else
		direction = Directions::Forward;
	if (parameters.count("speed"))
		speed = str2speed(parameters["speed"]);
	else
		speed = Speed::Normal;
	affectedElements.resize(group->size(), false);
	affectedElements.shrink_to_fit();
	Utility::checkAttributes(requiredParameters, parameters, "actor.");
	// default frames calculation.
	totalActorFrames = FPS * (static_cast<uint8_t>(speed) + 1) / 2;
	restart();
}

bool Actor::drawFrame() {

	const vector<bool>& affected = calculateElements();
	if (not affected.empty())
		switch (filter) {
		case Color::Filters::Mask:
			// turn off any other element.
			for (uint8_t elIdx = 0; elIdx < getNumberOfElements(); ++elIdx) {
				if (affected[elIdx])
					continue;
				changeElementColor(elIdx, Color::getColor("Black"), Color::Filters::Normal, 100);
			}
		}
	advanceActorFrame();
	return isLastFrame();
}

void Actor::drawConfig() {
	cout <<
		"Group: ["      << group->getName()          <<
		"], Speed: "    << speed2str(speed)          <<
		", Filter: "    << Color::filter2str(filter) <<
		", Direction: " << direction2str(direction)  << endl;
}

void Actor::restart() {
	if (isDirectionForward()) {
		currentActorFrame = 1;
		cDirection = Directions::Forward;
	}
	else {
		currentActorFrame = totalActorFrames;
		cDirection = Directions::Backward;
	}
}

string Actor::direction2str(Directions direction) {
	switch (direction) {
	case Directions::Stop:
		return "None";
	case Directions::Forward:
		return "Forward";
	case Directions::Backward:
		return "Backward";
	case Directions::ForwardBouncing:
		return "Forward Bouncing";
	case Directions::BackwardBouncing:
		return "Backward Bouncing";
	}
	return "";
}

Actor::Directions Actor::str2direction(const string& direction) {

	if (direction == "Stop")
		return Directions::Stop;
	if (direction == "Forward")
		return Directions::Forward;
	if (direction == "Backward")
		return Directions::Backward;
	if (direction == "ForwardBouncing")
		return Directions::ForwardBouncing;
	if (direction == "BackwardBouncing")
		return Directions::BackwardBouncing;
	throw Error("Invalid direction " + direction);
}

string Actor::speed2str(Speed speed) {
	switch (speed) {
	case Speed::VeryFast:
		return "Very Fast";
	case Speed::Fast:
		return "Fast";
	case Speed::Normal:
		return "Normal";
	case Speed::Slow:
		return "Slow";
	case Speed::VerySlow:
		return "Very Slow";
	}
	return "";
}

Actor::Speed Actor::str2speed(const string& speed) {
	if (speed == "VeryFast")
		return Speed::VeryFast;
	if (speed == "Fast")
		return Speed::Fast;
	if (speed == "Normal")
		return Speed::Normal;
	if (speed == "Slow")
		return Speed::Slow;
	if (speed == "VerySlow")
		return Speed::VerySlow;
	throw Error("Invalid speed " + speed);
}

void Actor::setFPS(uint8_t FPS) {
	LogDebug("Setting FPS to " + to_string(FPS));
	Actor::FPS = FPS;
}

uint8_t Actor::getFPS() {
	return FPS;
}

void Actor::advanceFrame() {
	++currentFrame;
	if (currentFrame > FPS)
		currentFrame = 1;
}

bool Actor::isBouncing() const {
	switch (direction) {
	case Directions::ForwardBouncing:
		return cDirection == Directions::Backward;
	case Directions::BackwardBouncing:
		return cDirection == Directions::Forward;
	default:
		return false;
	}
}

bool Actor::isBouncer() const {
	return direction == Directions::ForwardBouncing or direction == Directions::BackwardBouncing;
}

bool Actor::isFirstFrame() const {
	switch (direction) {
	case Directions::Forward:
	case Directions::ForwardBouncing:
		if (cDirection == Directions::Forward and currentActorFrame == 1)
			return true;
		break;
	case Directions::Backward:
	case Directions::BackwardBouncing:
		if (cDirection == Directions::Backward and currentActorFrame == totalActorFrames)
			return true;
		break;
	}
	return false;
}

bool Actor::isLastFrame() const {
	switch (direction) {
	case Directions::Forward:
	case Directions::BackwardBouncing:
		if (cDirection == Directions::Forward and currentActorFrame == totalActorFrames)
			return true;
		break;
	case Directions::Backward:
	case Directions::ForwardBouncing:
		if (cDirection == Directions::Backward and currentActorFrame == 1)
			return true;
		break;
	}
	return false;
}

bool Actor::isDirectionForward() {
	return direction == Directions::Forward or direction == Directions::ForwardBouncing;
}

bool Actor::isDirectionBackward() {
	return direction == Directions::Backward or direction == Directions::BackwardBouncing;
}

uint8_t Actor::getNumberOfElements() const {
	return group->getElements().size();
}

void Actor::changeElementColor(uint8_t index, const Color& color, Color::Filters filter, uint8_t percent) {
	Element* e = group->getElement(index);
	e->setColor(*e->getColor().set(color, filter, percent));
}

void Actor::changeElementsColor(const Color& color, Color::Filters filter, uint8_t percent) {
	for (uint8_t c = 0; c < group->size(); ++c) {
		changeElementColor(c, color, filter, percent);
	}
}

void Actor::advanceActorFrame() {
	currentActorFrame = calculateNextOf(cDirection, currentActorFrame, direction, totalActorFrames);
}

uint8_t Actor::calculateNextOf(Directions& currentDirection, uint8_t frame, Directions direction, uint8_t totalElements) {

	switch (direction) {
	case Directions::Forward:
		if (frame >= totalElements)
			frame = 1;
		else
			++frame;
		break;
	case Directions::ForwardBouncing:
	case Directions::BackwardBouncing:
		if (currentDirection == Directions::Forward) {
			// change of direction.
			if (frame >= totalElements) {
				currentDirection = Directions::Backward;
				--frame;
				break;
			}
			++frame;
			break;
		}
		if (frame <= 1) {
			currentDirection = Directions::Forward;
			++frame;
			break;
		}
		--frame;
		break;
	case Directions::Backward:
		if (frame <= 1)
			frame = totalElements;
		else
			--frame;
		break;
	}

	return frame;
}

vector<const LEDSpicer::Color*> Actor::extractColors(string colors) {
	vector<const Color*> r;
	for (auto& c : Utility::explode(colors, ',')) {
		Utility::trim(c);
		r.push_back(&Color::getColor(c));
	}
	r.shrink_to_fit();
	return r;
}

void Actor::affectAllElements(bool value) {
	affectedElements.assign(getNumberOfElements(), value);
}
