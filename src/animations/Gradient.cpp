/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Gradient.cpp
 * @since     Jul 3, 2018
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2018 - 2024 Patricio A. Rossi (MeduZa)
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
	Colors(parameters["colors"]),
	mode(str2mode(parameters["mode"])),
	tones(parameters.count("tones") ? Utility::parseNumber(parameters["tones"], "Invalid tones value, it need to be a number") : DEFAULT_TONES)
{
	// Add the first color to the end so the gradient ends in the same color that started.
	colors.push_back(colors.front());

	// Calculate speed and tones base on speed.
	tones *= static_cast<float>(speed) + 1;
	// More speed less tones.
	setTotalStepFrames(static_cast<float>(speed));

	float increment = (100.0 / tones) - 0.01;

	// Pre-calculate colors.
	uint8_t lastColor = colors.size() -1, nextColor;
	for (size_t color = 0; color < lastColor; ++color) {
		nextColor = color + 1;
		for (float percent = increment; percent < 100; percent += increment) {
			precalc.push_back(colors[color]->transition(*colors[nextColor], percent));
		}
	}
	// Return the colors to its normal shape, colors are only necessary for Draws().
	colors.pop_back();
	precalc.shrink_to_fit();
	totalFrames = precalc.size() - 1;
}

void Gradient::calculateElements() {

#ifdef DEVELOP
	cout << "Gradient: " << DrawDirection(cDirection) << " Frame " << static_cast<uint>(currentFrame) << endl;
#endif

	switch (mode) {
	case Modes::All:
		calculateSingle();
		break;
	case Modes::Cyclic:
		calculateMultiple();
		break;
	}
}

void Gradient::drawConfig() {
	cout <<
		"Type: Gradient"                     << endl <<
		"Mode: "  << mode2str(mode)          << endl <<
		"Tones: " << static_cast<int>(tones) << endl;
	StepActor::drawConfig();
	cout << "Colors: ";
	Color::drawColors(colors);
	cout << endl << SEPARATOR << endl;
}

Gradient::Modes Gradient::str2mode(const string& mode) {
	if (mode == "All")
		return Modes::All;
	if (mode == "Cyclic")
		return Modes::Cyclic;
	LogError("Invalid mode " + mode + " assuming Cyclic");
	return Modes::Cyclic;
}

string Gradient::mode2str(Modes mode) {
	switch (mode) {
	case Modes::All:
		return "All";
	case Modes::Cyclic:
		return "Cyclic";
	}
	return "";
}

void Gradient::calculateSingle() {
	// Paint all elements with the same color.
	for (uint8_t c = 0; c < getNumberOfElements(); c++) {
		changeElementColor(c, precalc[currentFrame], filter);
	}
}

void Gradient::calculateMultiple() {
	uint8_t frameT = currentFrame;
	// Paint each element with a tone.
	for (uint8_t c = 0; c < getNumberOfElements(); c++) {
		changeElementColor(c, precalc[frameT], filter);
		if (frameT == totalFrames)
			frameT = 0;
		else
			frameT++;
	}
}
