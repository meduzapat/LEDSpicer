/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Filler.cpp
 * @since     Oct 26, 2018
 * @author    Patricio A. Rossi (MeduZa)
 * @copyright Copyright © 2018 Patricio A. Rossi (MeduZa)
 */

#include "Filler.hpp"

using namespace LEDSpicer::Animations;

Filler::Filler(umap<string, string>& parameters, Group* const group) :
	TimedActor(parameters, group),
	color(parameters["color"])
{
	mode = str2mode(parameters["mode"]);
	totalActorFrames = group->size();
	restart();
	switch (mode) {
	case Modes::Linear:
		// need to check if the direction is bouncing and change cdirection for backward into forward
		filling = cDirection == Directions::Forward;
		break;
	case Modes::Random:
	case Modes::RandomSimple:
		std::srand(std::time(nullptr));
		elementsOn.resize(totalActorFrames, false);
		direction  = Directions::Forward;
		cDirection = direction;
	}
}

void Filler::calculateElements() {

	switch (mode) {
	case Modes::Linear:
		// using filling for backward.
		if (cDirection == Directions::Forward) {
			fillElementsLinear(
				//        forward             backward
				filling ? 0                 : totalActorFrames - currentActorFrame,
				filling ? currentActorFrame : totalActorFrames
			);
			if (willChange() and
				(((direction == Directions::ForwardBouncing or direction == Directions::Forward) and isLastFrame()) or
				((direction == Directions::BackwardBouncing or direction == Directions::Backward) and isFirstFrame())))
				filling = not filling;
			break;
		}
		fillElementsLinear(
			filling ? 0 : totalActorFrames - currentActorFrame ,
			filling ? currentActorFrame : totalActorFrames
		);
		break;
	case Modes::LinearSimple:
		if (direction == Directions::Forward or direction == Directions::ForwardBouncing)
			fillElementsLinear(0, currentActorFrame);
		else
			fillElementsLinear(currentActorFrame - 1, totalActorFrames);
		break;
	// Forward or backward will work in the same way for random modes.
	case Modes::Random:
		fillElementsRandom(filling);
		break;
	case Modes::RandomSimple:
		if (isFirstFrame() and not isSameFrame())
			std::fill(elementsOn.begin(), elementsOn.end(), false);
		fillElementsRandom(true);
		break;
	}
}

void Filler::fillElementsLinear(uint8_t begin, uint8_t end) {
	for (uint8_t c = begin; c != end; ++c)
		changeElementColor(c, color, filter);
}

void Filler::fillElementsRandom(bool val) {

	// Draw.
	if (isSameFrame()) {
		drawRandom();
		return;
	}

	// Extract candidates.
	vector<int8_t> posibleElements;
	for (uint8_t e = 0; e < elementsOn.size(); ++e)
		if (elementsOn[e] != val)
			posibleElements.push_back(e);

	// Roll dice.
	uint8_t r = 0;
	if (posibleElements.size() > 1)
		r = rand() / ((RAND_MAX + 1u) / (posibleElements.size() - 1));

	// Set dice and draw.
	elementsOn[posibleElements[r]] = val;
	drawRandom();

	// Check direction and force last frame.
	if (posibleElements.size() == 1) {
		filling = not filling;
		currentActorFrame = totalActorFrames;
	}
}

void Filler::drawRandom() {
	for (uint8_t e = 0; e < elementsOn.size(); ++e)
		if (elementsOn[e])
			changeElementColor(e, color, filter);
}

void Filler::drawConfig() {
	cout << "Actor Type: Filler, Mode: " << mode2str(mode) << endl;
	Actor::drawConfig();
	cout << "Color: ";
	color.drawColor();
}


string Filler::mode2str(Modes mode) {
	switch (mode) {
	case Modes::Linear:
		return "Linear";
	case Modes::Random:
		return "Random";
	case Modes::LinearSimple:
		return "Linear Simple";
	case Modes::RandomSimple:
		return "Random Simple";
	}
	return "";
}

Filler::Modes Filler::str2mode(const string& mode) {
	if (mode == "Linear")
		return Modes::Linear;
	if (mode == "Random")
		return Modes::Random;
	if (mode == "LinearSimple")
		return Modes::LinearSimple;
	if (mode == "RandomSimple")
		return Modes::RandomSimple;
	throw Error("Invalid mode " + mode);
}


