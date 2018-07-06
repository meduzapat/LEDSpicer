/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file		Gradient.cpp
 * @since		Jul 3, 2018
 * @author		Patricio A. Rossi (MeduZa)
 * @copyright	Copyright © 2018 Patricio A. Rossi (MeduZa)
 */

#include "Gradient.hpp"

using namespace LEDSpicer::Animations;

Gradient::Gradient(umap<string, string>& parameters, Group* const layout) :
	Actor(parameters, layout),
	mode(str2mode(parameters["mode"])),
	colorDirection(cDirection)
{
	for (auto& c : Utility::explode(parameters["colors"], ','))
		colors.push_back(Color::getColor(c));
	if (colors.size() < 2)
		throw Error("You need two or more colors for actor Gradient to do something.");
	totalActorFrames = FPS * changeFrameTotal / 2;
	currentColor = cDirection == Directions::Forward ? 1 : colors.size();
}

void Gradient::calculateElements() {
	switch (mode) {
	case Modes::All:
		calculateAll();
		break;
	case Modes::Sequential:
		calculateSequential();
		break;
	}
}

bool Gradient::canAdvaceFrame() {
	return true;
}

void Gradient::advanceActorFrame() {
	currentActorFrame = calculateNextFrame(cDirection, currentActorFrame, Directions::Forward, totalActorFrames);
}

void Gradient::drawConfig() {
	cout << "Actor Type: Gradient " << endl;
	Actor::drawConfig();
	cout << "Colors: " << endl;
	for (auto& c : colors)
		c.drawColor();
	cout << "Mode: " << mode2str(mode) << endl;
}

Gradient::Modes Gradient::str2mode(const string& mode) {
	if (mode == "All")
		return Modes::All;
	if (mode == "Sequential")
		return Modes::Sequential;
	Log::error("Invalid mode " + mode + " assuming All");
	return Modes::All;
}

string Gradient::mode2str(Modes mode) {
	switch (mode) {
	case Modes::All:
		return "All";
	case Modes::Sequential:
		return "Sequential";
	}
	return "";
}

void Gradient::calculateAll() {

	Directions currentColorDir = colorDirection;
	uint8_t nextColor = calculateNextFrame(currentColorDir, currentColor, direction, colors.size());
	uint8_t percent = currentActorFrame * 100 / totalActorFrames;
	changeElementsColor(colors[currentColor - 1].transition(colors[nextColor - 1], percent), filter);
	if (percent == 100)
		currentColor = calculateNextFrame(colorDirection, currentColor, direction, colors.size());
}

void Gradient::calculateSequential() {

	// change the 1st element and then copy increase the others.
	Directions currentColorDir = colorDirection;
	uint8_t
		nextColor = calculateNextFrame(currentColorDir, currentColor, direction, colors.size()),
		percent = currentActorFrame * 100 / totalActorFrames,
		nextElement = currentActorFrame + 1;
	changeElementColor(0, colors[currentColor - 1].transition(colors[nextColor - 1], percent), filter);
	if (percent == 100)
		currentColor = calculateNextFrame(colorDirection, currentColor, direction, colors.size());

	//  copy increase with fakes.
	currentColorDir = colorDirection;
	uint8_t currentColorTemp = currentColor;
	nextColor = calculateNextFrame(currentColorDir, currentColor, direction, colors.size());
	for (uint8_t c = 1; c < getNumberOfElements(); ++c) {
		percent = nextElement * 100 / totalActorFrames,
		changeElementColor(c, colors[currentColorTemp - 1].transition(colors[nextColor - 1], percent), filter);
		nextElement = calculateNextFrame(currentColorDir, nextElement, Directions::Forward, totalActorFrames);
		if (percent == 100) {
			currentColorTemp = calculateNextFrame(currentColorDir, currentColorTemp, direction, colors.size());
			nextColor = calculateNextFrame(currentColorDir, currentColorTemp, direction, colors.size());
		}
	}
}
