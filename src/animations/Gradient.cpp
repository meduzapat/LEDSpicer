/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Gradient.cpp
 * @since     Jul 3, 2018
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2018 - 2020 Patricio A. Rossi (MeduZa)
 *
 * @copyright LEDSpicer is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * @copyright LEDSpicer is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * @copyright You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "Gradient.hpp"

using namespace LEDSpicer::Animations;

Gradient::Gradient(umap<string, string>& parameters, Group* const group) :
	StepActor(parameters, group, REQUIRED_PARAM_ACTOR_GRADIENT),
	mode(str2mode(parameters["mode"]))
{

	// Number of tones between two colors.
	if (parameters.count("tones"))
		tones = Utility::parseNumber(parameters["tones"], "Invalid tones value, it need to be a number");

	totalStepFrames = totalStepFrames / 5;

	extractColors(parameters["colors"]);
	float
		percent = 0,
		increment;

	switch (mode) {

	case Modes::All:
	case Modes::Sequential:
		increment = tones ? 100 / tones : 100;
		break;

	case Modes::Cyclic:
		increment = 100 / (group->size() / static_cast<float>(colors.size()));
		break;
	}

	Directions colorDir = Directions::Forward;

	uint8_t
		lastColor = colors.size() - 1,
		color     = 0,
		nextColor = calculateNextOf(colorDir, color, Directions::Forward, lastColor);
	do {
		precalc.push_back(
			colors[color]->transition(
				*colors[nextColor],
				percent
			)
		);
		percent += increment;
		if (percent > 99) {
			percent   = 0;
			colorDir  = Directions::Forward;
			color     = nextColor;
			nextColor = calculateNextOf(colorDir, color, Directions::Forward, lastColor);
		}
	}
	while (precalc.size() != group->size());
	precalc.shrink_to_fit();
	totalFrames = precalc.size() - 1;
}

void Gradient::calculateElements() {
	switch (mode) {
	case Modes::All:
		calculateSingle();
		break;
	case Modes::Sequential:
	case Modes::Cyclic:
		calculateMultiple();
		break;
	}
}

void Gradient::drawConfig() {
	cout <<
		"Type: Gradient" <<  endl <<
		"Mode: " << mode2str(mode) << endl;
	StepActor::drawConfig();
	cout << "Colors: ";
	Color::drawColors(colors);
	cout << endl;
}

Gradient::Modes Gradient::str2mode(const string& mode) {
	if (mode == "All")
		return Modes::All;
	if (mode == "Sequential")
		return Modes::Sequential;
	if (mode == "Cyclic")
		return Modes::Cyclic;
	LogError("Invalid mode " + mode + " assuming All");
	return Modes::All;
}

string Gradient::mode2str(Modes mode) {
	switch (mode) {
	case Modes::All:
		return "All";
	case Modes::Sequential:
		return "Sequential";
	case Modes::Cyclic:
		return "Cyclic";
	}
	return "";
}

void Gradient::calculateSingle() {
	changeElementsColor(precalc[currentFrame], filter);
}

void Gradient::calculateMultiple() {

	uint8_t frameT = currentFrame;
	for (uint8_t c = 0; c < getNumberOfElements(); c++) {
		changeElementColor(
			c,
			precalc[frameT],
			filter
		);
		frameT++;
		if (frameT == precalc.size())
			frameT = 0;
	}
}

