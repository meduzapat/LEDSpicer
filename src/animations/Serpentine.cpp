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
	Actor(parameters, group),
	color(parameters["color"]),
	tailColor(parameters["tailColor"])
{
	totalActorFrames = group->size() * (direction == Directions::Forward or direction == Directions::Backward ? 1 : 2);
	tailDirection = cDirection == Directions::Forward ? Directions::Backward : Directions::Forward;
	tailData.resize(stoi(parameters["tailLength"]));
	tailData.shrink_to_fit();
	uint8_t tailweight = 100 / (tailData.size() + 1);
	if (tailData.size())
		for (uint8_t c = 0; c < tailData.size(); c++)
			tailData[c].percent = tailweight * (tailData.size() - c);
}

void Serpentine::calculateElements() {
	changeElementColor(currentActorFrame - 1, color, filter);
	calculateTailPosition();
	for (auto& data : tailData)
		changeElementColor(data.position - 1, tailColor, Color::Filters::Combine, data.percent);
}

void Serpentine::calculateTailPosition() {
	uint8_t lastTail = currentActorFrame;
	for (auto& ts : tailData)
		ts.position = lastTail = calculateNextFrame(tailDirection, lastTail);
}

void Serpentine::drawConfig() {
	cout << "Actor Type: Serpentine " << endl;
	Actor::drawConfig();
	cout << "Color: ";
	color.drawColor();
	cout << "Tail Color: ";
	tailColor.drawColor();
	cout << "Tail Length: " << (int)tailData.size() << endl;
}
