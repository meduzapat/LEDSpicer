/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file		Animation.cpp
 * @ingroup
 * @since		Jun 22, 2018
 * @author		Patricio A. Rossi (MeduZa)
 * @copyright	Copyright © 2018 Patricio A. Rossi (MeduZa)
 */

#include "Animation.hpp"


using namespace LEDSpicer::Animations;

Animation::Animation(umap<string, string>& parameters, Group& layout) :
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

void Animation::drawFrame() {
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

void Animation::drawConfig() {
	cout << "speed: " << (int)speed << "% filter: " << Color::filter2str(filter) << " ";
}

string Animation::direction2str(Directions direction) {
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

Animation::Directions Animation::str2direction(const string& direction) {

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

string Animation::effects2str(Effects effect) {
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

Animation::Effects Animation::str2effects(const string& effect) {
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
