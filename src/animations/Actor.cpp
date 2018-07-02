/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file		Actor.cpp
 * @since		Jun 22, 2018
 * @author		Patricio A. Rossi (MeduZa)
 * @copyright	Copyright © 2018 Patricio A. Rossi (MeduZa)
 */

#include "Actor.hpp"

using namespace LEDSpicer::Animations;

uint8_t Actor::currentFrame = 1;
uint8_t Actor::FPS          = 0;

Actor::Actor(umap<string, string>& parameters, Group* const group) :
	direction(str2direction(parameters["direction"])),
	filter(Color::str2filter(parameters["filter"])),
	group(group)
{

	if (direction == Directions::Forward or direction == Directions::ForwardBouncing) {
		cDirection = Directions::Forward;
	}
	else {
		cDirection = Directions::Backward;
		currentActorFrame = totalActorFrames;
	}
	changeFrameTotal = static_cast<uint8_t>(str2speed(parameters["speed"])) + 1;
}

uint8_t Actor::drawFrame() {

	calculateElements();

	if (canAdvaceFrame())
		advanceActorFrame();

	return currentActorFrame;
}

const uint8_t Actor::getTotalFrames() const {
	return totalActorFrames;
}

void Actor::drawConfig() {
	cout <<
		"Speed: " << speed2str(static_cast<Speed>(changeFrameTotal)) <<
		" Filter: " << Color::filter2str(filter) <<
		" Direction: " << direction2str(direction) << endl;
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
		return "VeryFast";
	case Speed::Fast:
		return "Fast";
	case Speed::Normal:
		return "Normal";
	case Speed::Slow:
		return "Slow";
	case Speed::VerySlow:
		return "VerySlow";
	}
	return "";
}

Actor::Speed Actor::str2speed(const string& speed) {
	if (speed == "Very Fast")
		return Speed::VeryFast;
	if (speed == "Fast")
		return Speed::Fast;
	if (speed == "Normal")
		return Speed::Normal;
	if (speed == "Slow")
		return Speed::Slow;
	if (speed == "Very Slow")
		return Speed::VerySlow;
	throw Error("Invalid speed " + speed);
}

void Actor::setFPS(uint8_t FPS) {
	Log::debug("Setting FPS to " + to_string(FPS));
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

uint8_t Actor::getNumberOfElements() const {
	return group->getElements().size();
}

void Actor::changeElementColor(uint8_t index, Color color, Color::Filters filter, uint8_t percent) {

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

void Actor::changeElementsColor(Color color, Color::Filters filter, uint8_t percent) {
	for (uint8_t c = 0; c < group->size(); ++c) {
		changeElementColor(c, color, filter, percent);
	}
}

bool Actor::canAdvaceFrame() {

	if (changeFrame >= changeFrameTotal) {
		changeFrame = 1;
		return true;
	}

	changeFrame++;
	return false;
}

void Actor::advanceActorFrame() {
	currentActorFrame = calculateNextFrame(cDirection, currentActorFrame);
}

uint8_t Actor::calculateNextFrame(Directions& cDirection, uint8_t frame) {

	switch (cDirection) {
	case Directions::Forward:
		if (frame == getNumberOfElements())
			frame = 1;
		else
			frame++;
		break;
	case Directions::ForwardBouncing:
	case Directions::BackwardBouncing:
		if (cDirection == Directions::Forward) {
			// change of direction.
			if (frame == getNumberOfElements()) {
				cDirection = Directions::Backward;
				frame--;
				break;
			}
			frame++;
			break;
		}
		if (frame == 1) {
			cDirection = Directions::Forward;
			frame++;
			break;
		}
		frame--;
		break;
	case Directions::Backward:
		if (frame == 1)
			frame = getNumberOfElements();
		else
			frame--;
		break;
	}

	return frame;
}
