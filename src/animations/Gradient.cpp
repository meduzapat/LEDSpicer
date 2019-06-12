/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Gradient.cpp
 * @since     Jul 3, 2018
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2018 - 2019 Patricio A. Rossi (MeduZa)
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
	TimedActor(parameters, group, REQUIRED_PARAM_ACTOR_GRADIENT),
	mode(str2mode(parameters["mode"])),
	colorDirection(cDirection)
{
	colors = extractColors(parameters["colors"]);
	if (colors.size() < 2)
		throw Error("You need two or more colors for actor Gradient to do something.");
	float steps;
	// Cyclic needs timedActor but others don't.
	switch (mode) {

	case Modes::All:
	case Modes::Sequential:
		steps = (static_cast<uint8_t>(speed) + 1) * 4;
		increment = 100 / steps;
		totalActorFrames = steps * colors.size();
		changeFrameTotal = 1;
		break;

	case Modes::Cyclic:
		steps = getNumberOfElements() / colors.size();
		increment = 100 / steps;
		break;
	}
	currentColor = cDirection == Directions::Forward ? 1 : colors.size();
	affectAllElements(true);
}

const vector<bool> Gradient::calculateElements() {
	switch (mode) {
	case Modes::All:
		calculateSingle();
		break;
	case Modes::Sequential:
	case Modes::Cyclic:
		calculateMultiple();
		break;
	}
	return affectedElements;
}

void Gradient::drawConfig() {
	cout << "Actor Type: Gradient" << ", Mode: " << mode2str(mode) << endl;
	Actor::drawConfig();
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

	Directions currentColorDir = colorDirection;
	uint8_t nextColor = calculateNextOf(currentColorDir, currentColor, direction, colors.size());
	changeElementsColor(
		colors[currentColor - 1]->transition(*colors[nextColor - 1],
		currentPercent),
		filter
	);
	currentPercent += increment;
	if (currentPercent >= 100) {
		currentPercent = 0;
		currentColor = calculateNextOf(colorDirection, currentColor, direction, colors.size());
	}
}

void Gradient::calculateMultiple() {

	Directions colorDir = colorDirection;

	uint8_t
		percent   = currentPercent,
		color     = currentColor,
		nextColor = calculateNextOf(colorDir, color, direction, colors.size());

	for (uint8_t c = 0; c < getNumberOfElements(); c++) {
		if (percent >= 100) {
			color     = nextColor;
			nextColor = calculateNextOf(colorDir, nextColor, direction, colors.size());
			percent   = 0;
		}

		changeElementColor(c, colors[color - 1]->transition(*colors[nextColor - 1], percent), filter);
		percent += increment;
	}
	if (willChange())
		currentPercent += increment;

	if (currentPercent >= 100) {
		currentColor = calculateNextOf(colorDirection, currentColor, direction, colors.size());
		currentPercent = 0;
	}
}

