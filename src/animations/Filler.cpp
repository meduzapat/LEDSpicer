/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Filler.cpp
 * @since     Oct 26, 2018
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

#include "Filler.hpp"

using namespace LEDSpicer::Animations;

Filler::Filler(umap<string, string>& parameters, Group* const group) :
	DirectionActor(parameters, group, REQUIRED_PARAM_ACTOR_FILLER),
	Color(parameters["color"]),
	mode(str2mode(parameters["mode"]))
{

	totalFrames = group->size();
	restart();
	switch (mode) {
	case Modes::Random:
	case Modes::RandomSimple:
		cDirection = direction  = Directions::Forward;
	}
}

const vector<bool> Filler::calculateElements() {

	switch (mode) {
	default: {
		uint8_t from, to;
		if (filling) {
			from = 1;
			to   = currentFrame;
		}
		else {
			from = currentFrame;
			to   = totalFrames;
		}
		fillElementsLinear(from, to);
		break;
	}
	// Forward or backward will work in the same way for random modes.
	case Modes::Random:
		fillElementsRandom(filling);
		break;
	case Modes::RandomSimple:
//		if (isFirstFrame() and not isSameFrame())
			affectAllElements();
		fillElementsRandom(true);
		break;
	}
	return affectedElements;
}

void Filler::fillElementsLinear(uint8_t begin, uint8_t end) {
	affectAllElements();
	for (uint8_t c = begin; c < end; ++c) {
		changeElementColor(c, *this, filter);
		affectedElements[c] = true;
	}
	//changeFrameElement(*this, true);
}

void Filler::fillElementsRandom(bool val) {

	// Draw.
/*	if (isSameFrame()) {
		drawRandom();
		return;
	}*/

	// Extract candidates.
	vector<uint8_t> possibleElements;
	for (uint8_t e = 0; e < affectedElements.size(); ++e)
		if (affectedElements[e] != val)
			possibleElements.push_back(e);

	// Roll dice.
	uint8_t r = 0;
	if (possibleElements.size() > 1)
		r = std::rand() / ((RAND_MAX + 1u) / (possibleElements.size() - 1));

	// Set dice and draw.
	affectedElements[possibleElements[r]] = val;
	drawRandom();

	// Check direction and force last frame.
	if (possibleElements.size() == 1) {
		filling = not filling;
		currentFrame = totalFrames;
	}
}

void Filler::drawRandom() {
	for (uint8_t e = 0; e < affectedElements.size(); ++e)
		if (affectedElements[e])
			changeElementColor(e, *this, filter);
}

void Filler::drawConfig() {
	cout <<
		"Type: Filler" << endl <<
		"Mode: " << mode2str(mode) << endl;
	DirectionActor::drawConfig();
	cout << "Color: ";
	this->drawColor();
}

string Filler::mode2str(Modes mode) {
	switch (mode) {
	case Modes::Linear:
		return "Linear";
	case Modes::Random:
		return "Random";
	case Modes::LinearSimple:
		return "Linear Simple";
	case Modes::RandomSimple:
		return "Random Simple";
	}
	return "";
}

Filler::Modes Filler::str2mode(const string& mode) {
	if (mode == "Linear")
		return Modes::Linear;
	if (mode == "Random")
		return Modes::Random;
	if (mode == "LinearSimple")
		return Modes::LinearSimple;
	if (mode == "RandomSimple")
		return Modes::RandomSimple;
	throw Error("Invalid mode " + mode);
}


