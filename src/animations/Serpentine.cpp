/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Serpentine.cpp
 * @since     Jun 22, 2018
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

#include "Serpentine.hpp"

using namespace LEDSpicer::Animations;

Serpentine::Serpentine(umap<string, string>& parameters, Group* const group) :
	StepActor(parameters, group, REQUIRED_PARAM_ACTOR_SERPENTINE),
	Color(parameters["color"]),
	tailColor(Color::getColor(parameters["tailColor"]))
{

	// Tail cannot be larger than the array, serpentine will overlap.
	uint8_t tailLength = Utility::parseNumber(
		parameters["tailLength"],
		"Invalid tailLength, enter a number 0 - " + to_string(totalFrames)
	);

	if (not Utility::verifyValue<uint8_t>(tailLength, 0, group->size()))
		throw Error("Tail cannot be larger than the group: " + to_string(totalFrames));

	if (not tailLength)
		return;

	uint8_t tailIntensity = Utility::parseNumber(parameters["tailIntensity"], "Invalid tailIntensity, enter a number 0-100");
	if (not Utility::verifyValue<uint8_t>(tailIntensity, 0, 100))
		throw Error("Invalid tailIntensity, enter a number 0-100");

	tailData.resize(tailLength);
	tailData.shrink_to_fit();

	float tailweight = tailIntensity / static_cast<float>(tailData.size() + 1), intensity = 0;
	for (auto& tail : tailData) {
		tail.percent = tailIntensity - intensity;
		if (direction == Directions::Forward)
			tail.position = 0;
		else
			tail.position = group->size() - 1;
		intensity += tailweight;
	}
}

void Serpentine::calculateElements() {

#ifdef DEVELOP
	cout << "Serpentine: " << DrawDirection(cDirection) << " Pos: " << static_cast<int>(currentFrame + 1) << " ";
#endif

	// For very fast, a much simple algorithm can be used.
	if (not tailData.size()) {
		if (speed == Speeds::VeryFast)
			changeElementColor(currentFrame, *this, filter);
		else
			changeFrameElement(*this, true, direction);
		return;
	}

	calculateTailPosition();
#ifdef DEVELOP
	cout << "Tail: ";
#endif
	for (auto& data : tailData) {
		switch (filter) {
		case Color::Filters::Mask:
			changeElementColor(data.position, tailColor.fade(data.percent), filter);
			break;
		default:
			changeElementColor(
				data.position,
				tailColor,
				Color::Filters::Combine,
				data.percent
			);
		}
#ifdef DEVELOP
		cout << static_cast<int>(data.position + 1) << "=" << static_cast<int>(data.percent) << "% ";
#endif
	}

	// For very fast, a much simple algorithm can be used.
	if (speed == Speeds::VeryFast)
		changeElementColor(currentFrame, *this, filter);
	else
		changeFrameElement(tailColor, *this, direction);
}

void Serpentine::calculateTailPosition() {
	Directions tailDirection = getOppositeDirection();
	uint8_t lastTail = currentFrame; //calculateNextOf(tailDirection, currentFrame, tailDirection, totalFrames);
	// Avoid changing the tail when doing the same frame.
	if (tailData[0].position == lastTail)
		return;
	for (uint8_t c = tailData.size() - 1; c > 0; --c)
		tailData[c].position = tailData[c - 1].position;
	tailData[0].position = lastTail;
}

void Serpentine::drawConfig() {
	cout << "Type: Serpentine " << endl;
	StepActor::drawConfig();
	cout << "Color: ";
	drawColor();
	cout << endl << "Tail: ";
	if (not tailData.size()) {
		cout << "No Tail" << endl << SEPARATOR << endl;
		return;
	}
	cout << " Color: ";
	tailColor.drawColor();
	cout <<
		", Length: " << static_cast<int>(tailData.size()) <<
		", Intensity: " << (tailData.front().percent + 0) <<
		"%" << endl
		<< SEPARATOR << endl;
}
