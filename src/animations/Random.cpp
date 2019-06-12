/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Random.cpp
 * @since     Jul 5, 2018
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

#include "Random.hpp"

using namespace LEDSpicer::Animations;

Random::Random(umap<string, string>& parameters, Group* const layout) :
	Actor(parameters, layout, REQUIRED_PARAM_ACTOR_RANDOM)
{

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
	affectAllElements(true);
}

const vector<bool> Random::calculateElements() {

	uint8_t percent = (currentActorFrame * 100) / totalActorFrames;

	for (uint8_t c = 0; c < getNumberOfElements(); ++c)
		changeElementColor(c, oldColors[c]->transition(*newColors[c], percent), filter);

	if (percent == 100) {
		oldColors = std::move(newColors);
		generateNewColors();
	}
	return affectedElements;
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
