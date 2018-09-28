/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file		Serpentine.cpp
 * @since		Jun 22, 2018
 * @author		Patricio A. Rossi (MeduZa)
 * @copyright	Copyright © 2018 Patricio A. Rossi (MeduZa)
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

	try {
		tailLength = stoi(parameters["tailLength"]);
		// tail cannot be larger than the array, serpentine will overlap.
		Utility::verifyValue(tailLength, 0, group->size());
	}
	catch (...) {
		throw Error("Invalid tailLength, enter a number 0-" + to_string(group->size()));
	}

	tailData.resize(tailLength);
	tailData.shrink_to_fit();
	try {
		tailIntensity = stoi(parameters["tailIntensity"]);
		Utility::verifyValue(tailIntensity, 0, 100);
	}
	catch (...) {
		throw Error("Invalid tailIntensity, enter a number 1-100");
	}
	float tailweight = tailIntensity / static_cast<float>(tailData.size() + 1);
	if (tailData.size())
		for (uint8_t c = 0; c < tailData.size(); c++) {
			tailData[c].percent  = tailweight * (tailData.size() - c);
			tailData[c].position = currentActorFrame;
		}
}

void Serpentine::calculateElements() {
	changeElementColor(currentActorFrame - 1, color, filter);
	calculateTailPosition();
	for (auto& data : tailData)
		changeElementColor(data.position - 1, tailColor, Color::Filters::Combine, data.percent);
}

void Serpentine::calculateTailPosition() {

	if (not tailData.size())
		return;

	Directions tailDirection = cDirection == Directions::Forward ? Directions::Backward : Directions::Forward;
	uint8_t lastTail = calculateNextOf(tailDirection, currentActorFrame, direction, totalActorFrames);
	// avoid duplicated frames due to speed.
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
