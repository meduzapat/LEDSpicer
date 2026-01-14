/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Gradient.cpp
 * @since     Jul 3, 2018
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

#include "Gradient.hpp"

using namespace LEDSpicer::Animations;

Gradient::Gradient(StringUMap& parameters, Group* const group) :
	StepActor(parameters, group, REQUIRED_PARAM_ACTOR_GRADIENT),
	Colors(parameters["colors"]),
	mode(str2mode(parameters["mode"])),
	tones(parameters.exists("tones") ? Utility::parseNumber(parameters["tones"], "Invalid tones value, it need to be a number") : DEFAULT_TONES)
{
	// Close the color loop by adding the first color at the end.
	colors.push_back(colors.front());

	// Precalculate transitions with integer steps for precision.
	// Each segment between colors has 'tones + 1' steps (including 0% and 100%).
	size_t numSegments = colors.size() - 1;
	precalc.reserve(numSegments * (tones + 1));

	for (size_t seg = 0; seg < numSegments; ++seg) {
		const Color& start = *colors[seg];
		const Color& end   = *colors[seg + 1];
		for (uint8_t step = 0; step <= tones; ++step) {
			float percent = static_cast<float>(step * 100.f) / tones;
			precalc.push_back(start.transition(end, percent));
		}
	}

	// Restore colors vector (remove looped end).
	colors.pop_back();
	// Set total frames based on precalc size.
	stepping.frames = precalc.size();
	// Adjust pacing: Higher speed means slower animation (more delay per frame).
	stepping.steps = static_cast<uint16_t>(speed);
}

void Gradient::calculateElements() {
#ifdef DEVELOP
	if (Log::isLogging(LOG_DEBUG)) {
		cout
			<< "Gradient: " << DrawDirection(cDirection.getDirection())
			<< " Frame "    << std::setw(2) << (stepping.frame + 1)
			<< "/"          << std::setw(2) << stepping.frames
			<< " Step "     << std::setw(1) << (stepping.step + 1)
			<< "/"          << std::setw(1) << stepping.steps << endl;
	}
#endif

	switch (mode) {
	case Modes::All:
		// Apply the same color to all elements.
		changeElementsColor(precalc[stepping.frame], filter);
		break;
	case Modes::Cyclic:
		calculateMultiple();
		break;
	}
}

void Gradient::drawConfig() const {
	cout <<
		"Type: Gradient"                     << endl <<
		"Mode: "  << mode2str(mode)          << endl <<
		"Tones: " << static_cast<int>(tones) << endl;
	cout << "Colors: ";
	Color::drawColors(colors);
	cout << endl;
	StepActor::drawConfig();
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

void Gradient::calculateMultiple() {
	// Distribute colors across elements, wrapping around the precalc.
	size_t numElems = getNumberOfElements();
	size_t frameIdx = stepping.frame;

	for (size_t elem = 0; elem < numElems; ++elem) {
		changeElementColor(elem, precalc[frameIdx], filter);
		frameIdx = (frameIdx + 1) % precalc.size();
	}
}
