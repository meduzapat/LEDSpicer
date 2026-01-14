/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Filler.cpp
 * @since     Oct 26, 2018
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

#include "Filler.hpp"

using namespace LEDSpicer::Animations;

Filler::Filler(StringUMap& parameters, Group* const group) :
	StepActor(parameters, group, REQUIRED_PARAM_ACTOR_FILLER),
	Colorful(parameters.exists("color") ? parameters["color"] : (parameters.exists("colors") ? parameters["colors"] : "")),
	mode(str2mode(parameters["mode"]))
{
	if (mode == Modes::Wave) {
		stepping.frames = group->size() * 2;
	}
	else if (mode == Modes::Curtain) {
		stepping.frames = group->size() / 2 + group->size() % 2;
	}
	else {
		stepping.frames = group->size();
	}
	if (mode == Modes::Random) {
		previousFrameAffectedElements.resize(stepping.frames, false);
		direction = Directions::Forward;
		cDirection.setDirection(Directions::Forward);
	}
	stepping.steps = calculateStepsBySpeed(speed);
	calculateStepPercent();
}

void Filler::generateNextRandom() {
	// Extract candidates.
	vector<uint16_t> possibleElements;
	for (uint16_t e = 0; e < stepping.frames; ++e)
		if (
			(not isBouncing() and not previousFrameAffectedElements[e]) or
			(isBouncing() and previousFrameAffectedElements[e])
		)
			possibleElements.push_back(e);

	if (not possibleElements.size())
		// None left
		return;
	else if (possibleElements.size() > 1)
		// Roll dice.
		currentRandom = possibleElements[std::rand() / ((RAND_MAX + 1u) / (possibleElements.size() - 1))];
	else
		// only one left
		currentRandom = possibleElements[0];
}

void Filler::calculateElements() {
#ifdef DEVELOP
	if (Log::isLogging(LOG_DEBUG)) {
		cout <<
			"Filler: " << DrawDirection(cDirection.getDirection());
			if (mode == Modes::Wave) {
				if (isBouncing())
					cout << (stepping.frame >= stepping.frames / 2 ? " Filling" : " Emptying");
				else
					cout << (stepping.frame >= stepping.frames / 2 ? " Emptying" : " Filling");
			}
			else {
				cout << (isBouncing() ? " Emptying" : " Filling");
			}
		cout <<
			" "        << colors[currentColor]->getName()          << " ";
	}
#endif

	switch (mode) {
	// Normal
	default:
	case Modes::Normal:
		fillElementsLinear();
		break;
	// Forward or backward will work in the same way for random modes.
	case Modes::Random:
		fillElementsRandom();
		if (isEndOfCycle()) generateNextRandom();
		break;
	case Modes::Wave:
		fillElementsWave();
		break;
	case Modes::Curtain:
		fillElementsCurtain();
		break;
	}

	if (not stepping.frame and not stepping.step and not isBouncing()) advanceColor();
}

void Filler::fillElementsLinear() {

	const Color* color(colors[currentColor]);

	// For very fast, a much simpler algorithm can be used.
	if (speed == Speeds::VeryFast)
		changeElementColor(stepping.frame, *color, filter);
	else
		changeFrameElement(stepping.frame, *color, getFadeEffectDirection());

	// Fill in between
	uint16_t begin, end;
	if (isForward()) {
		begin = 0;
		end   = stepping.frame;
	}
	else {
		begin = stepping.frame;
		end   = stepping.frames;
	}

	for (uint16_t c = begin; c < end; ++c)
		if (c != stepping.frame)
			changeElementColor(c, *color, filter);
}

void Filler::fillElementsRandom() {
	if (not stepping.step) generateNextRandom();
	const Color* color = colors[currentColor];
	// Fill already done elements.
	for (uint16_t e = 0; e < stepping.frames; ++e)
		if (previousFrameAffectedElements[e] and e != currentRandom)
			changeElementColor(e, *color, filter);

	if (speed == Speeds::VeryFast)
		changeElementColor(currentRandom, *color, filter);
	else
		changeFrameElement(currentRandom, *color, cDirection.getDirection());

	if (stepping.isLastStep()) {
		if (isLastFrame() and not isBouncer())
			std::fill(previousFrameAffectedElements.begin(), previousFrameAffectedElements.end(), false);
		else
			previousFrameAffectedElements[currentRandom] = not isBouncing();
	}
}

void Filler::fillElementsWave() {

	const Color* color(colors[currentColor]);
	uint16_t frames = stepping.frames / 2, frame, begin, end;
	bool isEmptying;
	Directions dir;

	auto setEmptyingParams = [&]() {
		frame = frames - (stepping.frames - stepping.frame);
		dir   = cDirection.getOppositeDirection();
		begin = frame;
		end   = frames;
	};

	auto setFillingParams = [&]() {
		frame = stepping.frame;
		dir   = cDirection.getDirection();
		begin = 0;
		end   = frame;
	};

	if (isForward()) {
		isEmptying = stepping.frame >= frames;
		if (isEmptying) setEmptyingParams();
		else setFillingParams();
	}
	else {
		isEmptying = stepping.frame < frames;
		if (isEmptying) setFillingParams();
		else setEmptyingParams();
	}

	// For very fast, a much simpler algorithm can be used.
	if (speed == Speeds::VeryFast)
		changeElementColor(frame, *color, filter);
	else
		changeFrameElement(frame, *color, dir);

	// Fill / Emptying in between
	for (uint16_t c = begin; c < end; ++c)
		if (c != frame) changeElementColor(c, *color, filter);
}

void Filler::fillElementsCurtain() {
	const Color* color(colors[currentColor]);
	// Calculate left and right positions based on current frame
	uint16_t rightPos = getNumberOfElements() - 1 - stepping.frame;
	Directions cd = getFadeEffectDirection();

	// For very fast, use simple algorithm
	if (speed == Speeds::VeryFast) {
		changeElementColor(stepping.frame, *color, filter);
		changeElementColor(rightPos, *color, filter);
	}
	else {
		changeFrameElement(stepping.frame, *color, cd);
		if (stepping.frame != rightPos)
			changeFrameElement(rightPos, *color, cd);
	}

	// Fill in between based on direction
	if (isForward()) {
		// Inward: fill from edges to current positions
		for (uint16_t c = 0; c < stepping.frame; ++c) {
			uint16_t mirror = getNumberOfElements() - 1 - c;
			changeElementColor(c, *color, filter);
			changeElementColor(mirror, *color, filter);
		}
	}
	else {
		// Outward: fill the whole middle.
		for (uint16_t c = stepping.frame + 1; c < rightPos; ++c) {
			changeElementColor(c, *color, filter);
		}
	}
}

void Filler::drawConfig() const {
	cout <<
		"Type: Filler"                   << endl <<
		"Mode: "       << mode2str(mode) << endl <<
		"Colors: ";
	this->drawColors();
	cout << endl;
	StepActor::drawConfig();
}

string Filler::mode2str(const Modes mode) {
	switch (mode) {
	case Modes::Normal:  return "Normal";
	case Modes::Random:  return "Random";
	case Modes::Wave:    return "Wave";
	case Modes::Curtain: return "Curtain";
	}
	return "";
}

Filler::Modes Filler::str2mode(const string& mode) {
	if (mode == "Normal")  return Modes::Normal;
	if (mode == "Random")  return Modes::Random;
	if (mode == "Wave")    return Modes::Wave;
	if (mode == "Curtain") return Modes::Curtain;
	throw Error("Invalid mode ") << mode;
}

void Filler::restart() {
	currentColor = colors.size() - 1;
	StepActor::restart();
	switch (mode) {
	case Modes::Random:
		std::fill(previousFrameAffectedElements.begin(), previousFrameAffectedElements.end(), false);
		break;
	case Modes::Curtain:
		if (isBackward()) stepping.frame = stepping.frames - 1;
		break;
	default: break;
	}
}

Direction::Directions Filler::getFadeEffectDirection() const {
	return isForward() ? cDirection.getDirection() : cDirection.getOppositeDirection();
}
