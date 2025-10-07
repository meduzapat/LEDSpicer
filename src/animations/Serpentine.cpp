/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Serpentine.cpp
 * @since     Jun 22, 2018
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

#include "Serpentine.hpp"

using namespace LEDSpicer::Animations;

actorFactory(Serpentine)

Serpentine::Serpentine(StringUMap& parameters, Group* const group) :
	StepActor(parameters, group, REQUIRED_PARAM_ACTOR_SERPENTINE),
	Color(Color::getColor(parameters["color"])),
	tailColor(Color::getColor(parameters.exists("tailColor") ? parameters.at("tailColor") : "Off"))
{
	stepping.frames = group->size();
	stepping.steps  = calculateStepsBySpeed(speed);
	calculateStepPercent();
	// Tail cannot be larger than the array, serpentine will overlap.
	uint16_t tailLength = Utility::parseNumber(
		parameters.exists("tailLength") ? parameters.at("tailLength") : "0",
		"Invalid tailLength, enter a number 0 - " + to_string(stepping.frames)
	);

	if (not Utility::verifyValue<uint16_t>(tailLength, 0, stepping.frames))
		throw Error("Tail cannot be larger than the group: ") << stepping.frames;

	if (not tailLength) return;

	uint8_t tailIntensity = Utility::parseNumber(parameters["tailIntensity"], "Invalid tailIntensity, enter a number 0-100");
	if (not Utility::verifyValue<uint8_t>(tailIntensity, 0, 100))
		throw Error("Invalid tailIntensity, enter a number 0-100");

	tailData.resize(tailLength);

	// Higher values = steeper decay
	const float decayFactor = 3.f;
	const float baseIntensity = tailIntensity;

	for (size_t i = 0; i < tailData.size(); ++i) {
		// Exponential decay: intensity = base * e^(-factor * position/length)
		const float normalizedPosition = static_cast<float>(i) / static_cast<float>(tailData.size());
		const float intensity = baseIntensity * std::exp(-decayFactor * normalizedPosition);

		tailData[i].percent = intensity;

		if (cDirection.isForward())
			tailData[i].position = 0;
		else
			tailData[i].position = stepping.getLastFrame();
	}
}

void Serpentine::calculateElements() {

#ifdef DEVELOP
	if (Log::isLogging(LOG_DEBUG)) {
		cout << "Serpentine: " << DrawDirection(cDirection.getDirection()) << " ";
	}
#endif

	if (not tailData.size()) {
		// For very fast, a much simpler algorithm can be used.
		if (speed == Speeds::VeryFast) // and not bounce
			changeElementColor(stepping.frame, *this, filter);
		else
			changeFrameElement(*this, true, direction);
		return;
	}

	calculateTailPosition();
#ifdef DEVELOP
	if (Log::isLogging(LOG_DEBUG)) {
		cout << "Tail: ";
	}
#endif
	for (size_t i = 1; i < tailData.size(); ++i) {
		auto data(tailData[i]);
		if (data.position == stepping.frame) {
#ifdef DEVELOP
			if (Log::isLogging(LOG_DEBUG)) {
				cout << std::setw(2) << static_cast<uint16_t>(data.position + 1) << "=--% ";
			}
#endif
			continue;
		}
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
	if (Log::isLogging(LOG_DEBUG)) {
		cout << std::setw(2)        << static_cast<uint16_t>(data.position + 1)
			 << "=" << std::setw(2) << static_cast<uint16_t>(data.percent) << "% ";
	}
#endif
	}

	// For very fast, a much simpler algorithm can be used.
	if (speed == Speeds::VeryFast) {
#ifdef DEVELOP
	if (Log::isLogging(LOG_DEBUG)) {
		cout << endl;
	}
#endif
		changeElementColor(stepping.frame, *this, filter);
	}
	else {
		changeFrameElement(tailColor, *this, direction);
	}
}

void Serpentine::calculateTailPosition() {
	// only calculate on new frames.
	if (stepping.step) return;
	uint16_t lastTail = stepping.frame;
	for (uint16_t c = tailData.size() - 1; c > 0; --c)
		tailData[c].position = tailData[c - 1].position;
	tailData[0].position = lastTail;
}

void Serpentine::drawConfig() const {
	cout <<
		"Type: Serpentine "          << endl <<
		"Color: " << this->getName() << endl <<
		"Tail: ";
	if (not tailData.size()) {
		cout << "No Tail" << endl;
	}
	else {
		cout <<
			" Color: "      << tailColor.getName()               <<
			", Length: "    << static_cast<int>(tailData.size()) <<
			", Intensity: " << (tailData.front().percent + 0)    <<
			"%" << endl;
	}
	StepActor::drawConfig();
}
