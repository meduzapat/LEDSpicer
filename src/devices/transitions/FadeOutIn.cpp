/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      FadeOutIn.cpp
 * @since     Sep 22, 2025
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

#include "FadeOutIn.hpp"

using namespace LEDSpicer::Devices::Transitions;

void FadeOutIn::drawConfig() const {
	cout << "Transition: FadeInOut" << endl;
	ProgressiveTransition::drawConfig();
	cout << "Color: " << getName() << endl;
}

void FadeOutIn::calculate() {
	// Split duration: half for fade-out, half for fade-in.
	int total = to ? 50 : 100;
	const float	invTotal = 1.0f / static_cast<float>(total);
	float factor;

	if (progress < total) {
		current->runFrame();
		// fade out
		factor = 1.0f - (progress * invTotal);
	}
	else {
		// This will never executes if `to` is null.
		to->runFrame();
		// fade in
		factor = (progress - total) * invTotal;
	}

	// Smoothstep interpolation (0 → 1)
	factor = factor * factor * (3.0f - 2.0f * factor);

	// Convert to percentage (0 → 100) for setColor
	const uint8_t fade = static_cast<uint8_t>((1.0f - factor) * 100.0f);

	// Apply fade factor to all elements
	for (const auto& [_, element] : Element::allElements) {
		element->setColor(*this, Color::Filters::Combine, fade);
	}
}
