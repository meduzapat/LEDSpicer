/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Random.cpp
 * @since     Jul 5, 2018
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2018 - 2026 Patricio A. Rossi (MeduZa)
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

actorFactory(Random)

Random::Random(StringUMap& parameters, Group* const group) :
	FrameActor(parameters, group, REQUIRED_PARAM_ACTOR_RANDOM),
	Colors(parameters.exists("colors") ? parameters["colors"] : "")
{
	const Color* blackColor = &Color::Off;
	oldColors.resize(group->size(), blackColor);
	stepping.frames = calculateFramesBySpeed(speed);
}

void Random::calculateElements() {

	uint8_t percent = PERCENT(stepping.frame, stepping.frames);
#ifdef DEVELOP
	if (Log::isLogging(LOG_DEBUG)) {
		cout << "Random: Frame: " << std::setw(3) << (stepping.frame + 1) << std::setw(4) << to_string(percent) << "%" << endl;
	}
#endif
	for (uint16_t c = 0; c < getNumberOfElements(); ++c) {
		changeElementColor(c, oldColors[c]->transition(*newColors[c], percent), filter);
	}

	if (isLastFrame()) {
		oldColors = std::move(newColors);
		generateNewColors();
	}
}

void Random::generateNewColors() {
	newColors.clear();
	for (uint16_t c = 0; c < getNumberOfElements(); ++c)
		newColors.push_back(colors[std::rand() / ((RAND_MAX + 1u) / colors.size())]);
}

void Random::drawConfig() const {
	cout << "Type: Random " << endl;
	cout << "Colors: ";
	Color::drawColors(colors);
	FrameActor::drawConfig();
}

void Random::restart() {
	FrameActor::restart();
	generateNewColors();
}


