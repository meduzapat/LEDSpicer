/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file		Random.cpp
 * @since		Jul 5, 2018
 * @author		Patricio A. Rossi (MeduZa)
 * @copyright	Copyright © 2018 Patricio A. Rossi (MeduZa)
 */

#include "Random.hpp"

using namespace LEDSpicer::Animations;

Random::Random(umap<string, string>& parameters, Group* const layout) :
	Actor(parameters, layout)
{

	try {
		std::srand(stoi(parameters["seed"]));
	}
	catch (...) {
		throw Error("Invalid number for seed");
	}

	oldColors.reserve(getNumberOfElements());

	if (parameters["colors"].empty()) {
		colors.reserve(Color::getNames().size());
		for (auto& c : Color::getNames())
			colors.push_back(&Color::getColor(c));
	}
	else {
		colors = extractColors(parameters["colors"]);
		if (colors.size() < 2)
			throw Error("You need two or more colors for actor Random to do something.");
	}

	for (uint8_t c = 0; c < getNumberOfElements(); ++c)
		oldColors.push_back(&Color::getColor("Black"));

	generateNewColors();
}

void Random::calculateElements() {

	uint8_t percent = (currentActorFrame * 100) / totalActorFrames;

	for (uint8_t c = 0; c < getNumberOfElements(); ++c)
		changeElementColor(c, oldColors[c]->transition(*newColors[c], percent), filter);

	if (percent == 100) {
		oldColors = std::move(newColors);
		generateNewColors();
	}
}

void Random::generateNewColors() {
	newColors.reserve(getNumberOfElements());
	for (uint8_t c = 0; c < getNumberOfElements(); ++c)
		newColors.push_back(colors[std::rand() / ((RAND_MAX + 1u) / colors.size())]);
}

void Random::advanceActorFrame() {
	currentActorFrame = calculateNextOf(cDirection, currentActorFrame, Directions::Forward, totalActorFrames);
}

void Random::drawConfig() {
	cout << "Actor Type: Random " << endl;
	Actor::drawConfig();
	cout << "Colors: ";
	Color::drawColors(colors);
	cout << endl;
}
