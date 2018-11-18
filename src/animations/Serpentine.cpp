/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Serpentine.cpp
 * @since     Jun 22, 2018
 * @author    Patricio A. Rossi (MeduZa)
 * @copyright Copyright © 2018 Patricio A. Rossi (MeduZa)
 */

#include "Serpentine.hpp"

using namespace LEDSpicer::Animations;

Serpentine::Serpentine(umap<string, string>& parameters, Group* const group) :
	TimedActor(parameters, group),
	color(parameters["color"]),
	tailColor(parameters["tailColor"])
{

	totalActorFrames = group->size();
	uint8_t
		tailLength    = 0,
		tailIntensity = 0;

	tailLength = Utility::parseNumber(
		parameters["tailLength"],
		"Invalid tailLength, enter a number 0-" + to_string(group->size())
	);
	// tail cannot be larger than the array, serpentine will overlap.
	if (not Utility::verifyValue<uint8_t>(tailLength, 0, group->size()))
		throw Error("Tail cannot be larger than the group: " + to_string(group->size()));

	tailData.resize(tailLength);
	tailData.shrink_to_fit();
	tailIntensity = Utility::parseNumber(parameters["tailIntensity"], "Invalid tailIntensity, enter a number 0-100");
	if (not Utility::verifyValue<uint8_t>(tailIntensity, 0, 100))
		throw Error("Invalid tailIntensity, enter a number 0-100");

	float tailweight = tailIntensity / static_cast<float>(tailData.size() + 1);
	Directions tailDirection = cDirection == Directions::Forward ? Directions::Backward : Directions::Forward;
	uint8_t firstTail = calculateNextOf(tailDirection, currentActorFrame, tailDirection, totalActorFrames);
	if (tailData.size())
		for (uint8_t c = 0; c < tailData.size(); c++) {
			tailData[c].percent  = tailweight * (tailData.size() - c);
			tailData[c].position = firstTail - c;
		}
}

const vector<bool> Serpentine::calculateElements() {

	vector<bool> elements(getNumberOfElements(), false);
	elements[currentActorFrame -1] = true;

	changeElementColor(currentActorFrame - 1, color, filter);
	calculateTailPosition();
	for (auto& data : tailData) {
		switch (filter) {
		case Color::Filters::Mask:
			changeElementColor(data.position - 1, tailColor.fade(data.percent), filter);
			break;
		default:
			changeElementColor(data.position - 1, tailColor, filter, data.percent);
		}
		elements[data.position -1] = true;
	}

	elements.shrink_to_fit();
	return elements;
}

void Serpentine::calculateTailPosition() {

	if (not tailData.size())
		return;

	Directions tailDirection = cDirection == Directions::Forward ? Directions::Backward : Directions::Forward;
	uint8_t lastTail = calculateNextOf(tailDirection, currentActorFrame, tailDirection, totalActorFrames);
	// Avoid changing the tail when doing the same frame.
	if (tailData[0].position == lastTail)
		return;
	for (uint8_t c = tailData.size() - 1; c > 0; --c)
		tailData[c].position = tailData[c - 1].position;
	tailData[0].position = lastTail;
}

void Serpentine::drawConfig() {
	cout << "Actor Type: Serpentine " << endl;
	Actor::drawConfig();
	cout << "Color: ";
	color.drawColor();
	cout << ", Tail Color: ";
	tailColor.drawColor();
	cout << ", Tail Length: " << (int)tailData.size() <<
			", Tail Intensity: " << (tailData.size() ? tailData.front().percent + tailData.back().percent : 0) <<
			"%" << endl;
}
