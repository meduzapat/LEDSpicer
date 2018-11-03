/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Actor.cpp
 * @since     Jun 22, 2018
 * @author    Patricio A. Rossi (MeduZa)
 * @copyright Copyright © 2018 Patricio A. Rossi (MeduZa)
 */

#include "Actor.hpp"

using namespace LEDSpicer::Animations;

uint8_t Actor::currentFrame = 1;
uint8_t Actor::FPS          = 0;

Actor::Actor(umap<string, string>& parameters, Group* const group) :
	direction(str2direction(parameters["direction"])),
	filter(Color::str2filter(parameters["filter"])),
	group(group),
	speed(str2speed(parameters["speed"]))
{
	// default frames calculation.
	totalActorFrames = FPS * (static_cast<uint8_t>(speed) + 1) / 2;
	restart();
}

bool Actor::drawFrame() {

	calculateElements();
	advanceActorFrame();
	if ((direction == Directions::Forward or direction == Directions::ForwardBouncing) and currentActorFrame == 1)
		return true;
	if ((direction == Directions::Backward or direction == Directions::BackwardBouncing) and currentActorFrame == totalActorFrames)
		return true;
	return false;
}

void Actor::drawConfig() {
	cout <<
		"Speed: "       << speed2str(speed)          <<
		", Filter: "    << Color::filter2str(filter) <<
		", Direction: " << direction2str(direction)  << endl;
}

void Actor::restart() {
	if (direction == Directions::Forward or direction == Directions::ForwardBouncing) {
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
	case Directions::ForwardBouncing:
		if (cDirection == Directions::Forward and currentActorFrame == totalActorFrames)
			return true;
		break;
	case Directions::Backward:
	case Directions::BackwardBouncing:
		if (cDirection == Directions::Backward and currentActorFrame == 1)
			return true;
		break;
	}
	return false;
}

uint8_t Actor::getNumberOfElements() const {
	return group->getElements().size();
}

void Actor::changeElementColor(uint8_t index, const Color& color, Color::Filters filter, uint8_t percent) {

	switch (filter) {
	case Color::Filters::Normal:
		group->getElement(index)->setColor(color);
		break;
	case Color::Filters::Combine:
		group->getElement(index)->setColor(
			group->getElement(index)->getColor().transition(color, percent)
		);
	}
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
