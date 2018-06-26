/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file		Actor.cpp
 * @since		Jun 22, 2018
 * @author		Patricio A. Rossi (MeduZa)
 * @copyright	Copyright © 2018 Patricio A. Rossi (MeduZa)
 */

#include "Actor.hpp"


using namespace LEDSpicer::Animations;

Actor::Actor(umap<string, string>& parameters, Group& layout) :
	speed(stoi(parameters["speed"])),
	filter(Color::str2filter(parameters["filter"])),
	group(layout)
{
	uint8_t pinC = 0;
	for (auto element : group.elements)
		pinC += element.pins.size();
	internalValues.resize(pinC, 0);
	internalValues.shrink_to_fit();
}

void Actor::drawFrame() {
	calculateFrame();
	uint8_t pinC = 0;
	for (auto element : group.elements) {
		for (auto pin : element.getPins()) {
			switch (filter) {
			case Color::Filters::Normal:
				*pin = internalValues[pinC];
			break;
			case Color::Filters::Combine:
				*pin = Color::transition(*pin, internalValues[pinC], 50);
			}
			++pinC;
		}
	}
}

void Actor::drawConfig() {
	cout << "Speed: " << (int)speed << "% Filter: " << Color::filter2str(filter) << " ";
}

string Actor::direction2str(Directions direction) {
	switch (direction) {
	case Stop:
		return "None";
	case Forward:
		return "Forward";
	case Backward:
		return "Backward";
	case ForwardBouncing:
		return "Forward Bouncing";
	case BackwardBouncing:
		return "Backward Bouncing";
	}
	return "";
}

Actor::Directions Actor::str2direction(const string& direction) {

	if (direction == "Stop")
		return Stop;
	if (direction == "Forward")
		return Forward;
	if (direction == "Backward")
		return Backward;
	if (direction == "ForwardBouncing")
		return ForwardBouncing;
	if (direction == "BackwardBouncing")
		return BackwardBouncing;
	throw Error("Invalid direction " + direction);
}

string Actor::effects2str(Effects effect) {
	switch (effect) {
	case None:
		return "None";
	case SlowFade:
		return "SlowFade";
	case Fade:
		return "Fade";
	case FastFade:
		return "FastFade";
	}
	return "";
}

Actor::Effects Actor::str2effects(const string& effect) {
	if (effect == "None")
		return None;
	if (effect == "SlowFade")
		return SlowFade;
	if (effect == "Fade")
		return Fade;
	if (effect == "FastFade")
		return FastFade;
	throw Error("Invalid Effect " + effect);
}
