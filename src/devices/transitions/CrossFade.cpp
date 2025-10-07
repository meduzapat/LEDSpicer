/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      CrossFade.cpp
 * @since     Sep 23, 2025
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

#include "CrossFade.hpp"

using namespace LEDSpicer::Devices::Transitions;

void CrossFade::drawConfig() const {
	cout << "Transition: CrossFade" << endl;
	ProgressiveTransition::drawConfig();
}

void CrossFade::calculate() {
	// Calculate blend factor (0 → 1)
	float factor = progress * (1.0f / 100.0f);
	factor = factor * factor * (3.0f - 2.0f * factor);

	const float fadeOutFactor = 1.0f - factor;
	const float fadeInFactor  = factor;

	// Render current with fade out
	current->runFrame();
	for (size_t i = 0, s = allLeds.size(); i < s; ++i) {
		cachedBuffer[i] = static_cast<uint8_t>(*allLeds[i] * fadeOutFactor);
	}

	// Render target and blend directly
	if (to) {
		to->runFrame(false);
		for (size_t i = 0, s = allLeds.size(); i < s; ++i) {
			const int targetValue = static_cast<int>(*allLeds[i] * fadeInFactor);
			*allLeds[i] = static_cast<uint8_t>(
				std::min(255, static_cast<int>(cachedBuffer[i]) + targetValue)
			);
		}
	}
	else {
		// Just copy cached buffer if no target
		for (size_t i = 0, s = allLeds.size(); i < s; ++i) {
			*allLeds[i] = cachedBuffer[i];
		}
	}
}
