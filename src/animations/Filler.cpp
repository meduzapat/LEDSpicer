/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Filler.cpp
 * @since     Oct 26, 2018
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

#include "Filler.hpp"

using namespace LEDSpicer::Animations;

Filler::Filler(umap<string, string>& parameters, Group* const group) :
	DirectionActor(parameters, group, REQUIRED_PARAM_ACTOR_FILLER),
	Color(parameters["color"]),
	mode(str2mode(parameters["mode"]))
{

	totalFrames = group->size() - 1;
	previousFrameAffectedElements.resize(totalFrames, false);
	previousFrameAffectedElements.shrink_to_fit();
}

void Filler::calculateElements() {

	switch (mode) {
	case Modes::LinearSimple: {
		uint8_t from, to;
		if (cDirection == Directions::Forward) {
			from = 0;
			to   = currentFrame;
		}
		else {
			from = currentFrame;
			to   = totalFrames;
		}
		fillElementsLinear(from, to);
		break;
	}
	case Modes::Linear: {
		uint8_t from, to;
		switch (direction) {
		case Directions::Forward:
			if (filling) {
				from = 0;
				to   = currentFrame;
			}
			else {
				from = currentFrame;
				to   = totalFrames;
			}
			break;
		case Directions::Backward:
			if (filling) {
				from = currentFrame;
				to   = totalFrames;
			}
			else {
				from = 0;
				to   = currentFrame;
			}
			break;
		case Directions::ForwardBouncing:
			if (filling) {
				if (cDirection == Directions::Forward) {
					from = 0;
					to   = currentFrame;
				}
				else {
					from = totalFrames - currentFrame;
					to   = totalFrames;
				}
			}
			else {
				if (cDirection == Directions::Forward) {
					from = totalFrames - currentFrame;
					to   = totalFrames;
				}
				else {
					from = 0;
					to   = currentFrame;
				}
			}
			break;
		case Directions::BackwardBouncing:
			if (cDirection == Directions::Forward) {
				if (filling) {
					from = 0;
					to   = totalFrames - currentFrame;
				}
				else {
					from = currentFrame;
					to   = totalFrames;
				}
			}
			else {
				if (filling) {
					from = currentFrame;
					to   = totalFrames;
				}
				else {
					from = 0;
					to   = totalFrames - currentFrame;
				}
			}
			break;
		}

		fillElementsLinear(from, to);
		break;
	}
	// Forward or backward will work in the same way for random modes.
	case Modes::Random:
		fillElementsRandom(filling);
		break;
	case Modes::RandomSimple:
		if (isFirstFrame())
			previousFrameAffectedElements.assign(previousFrameAffectedElements.size(), false);
		fillElementsRandom(true);
		break;
	}
}

void Filler::fillElementsLinear(uint8_t begin, uint8_t end) {
#ifdef DEVELOP
	cout << "Filler Linear ";
#endif
	for (uint8_t c = begin; c <= end; ++c) {
		changeElementColor(c, *this, filter);
#ifdef DEVELOP
		cout << static_cast<int>(c) << " ";
#endif

	}
	if (isLastFrame())
		filling = not filling;
#ifdef DEVELOP
	cout << endl;
#endif
}

void Filler::fillElementsRandom(bool val) {

	// Extract candidates.
	vector<uint8_t> possibleElements;
	for (uint8_t e = 0; e < totalFrames; ++e)
		if (previousFrameAffectedElements[e] != val)
			possibleElements.push_back(e);

	// Roll dice.
	uint8_t r = 0;
	if (possibleElements.size() > 1) {
		r = std::rand() / ((RAND_MAX + 1u) / (possibleElements.size() - 1));
		// Set dice and draw.
		previousFrameAffectedElements[possibleElements[r]] = val;
	}
	drawRandom();

	// Check direction and force last frame.
	if (possibleElements.size() == 1) {
		filling = not filling;
		currentFrame = totalFrames;
	}
}

void Filler::drawRandom() {
#ifdef DEVELOP
	cout << "Filler Random ";
#endif
	for (uint8_t e = 0; e < totalFrames; ++e)
		if (previousFrameAffectedElements[e]) {
			changeElementColor(e, *this, filter);
#ifdef DEVELOP
			cout << static_cast<int>(e) << " ";
#endif
		}
#ifdef DEVELOP
	cout << endl;
#endif
}

void Filler::drawConfig() {
	cout <<
		"Type: Filler" << endl <<
		"Mode: " << mode2str(mode) << endl;
	DirectionActor::drawConfig();
	cout << "Color: ";
	this->drawColor();
	cout << SEPARATOR << endl;
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

void Filler::restart() {
	DirectionActor::restart();
	switch (mode) {
	case Modes::Random:
	case Modes::RandomSimple:
		cDirection = direction  = Directions::Forward;
		break;
	}
}
