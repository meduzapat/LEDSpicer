/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      CrossFade.hpp
 * @since     Sep 23, 2025
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2018 - 2025 Patricio A. Rossi (MeduZa)
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

#include "ProgressiveTransition.hpp"
#include "utilities/Color.hpp"

#pragma once

namespace LEDSpicer::Devices::Transitions {

/**
 * LEDSpicer::Devices::Transitions::CrossFade
 * A transition that crossfades from the current profile to the target profile.
 */
class CrossFade : public ProgressiveTransition {

public:

	/**
	 * @param current
	 * @param to
	 * @param speed
	 * @param color
	 */
	CrossFade(Profile* current, const string& speed) :
		ProgressiveTransition(current, speed),
		allLeds(Group::layout.at("All").getLeds())
	{
		cachedBuffer.resize(allLeds.size());
	}

	virtual ~CrossFade() = default;

	/**
	 * @see Transition::drawConfig()
	 */
	void drawConfig() const override;

protected:

	/// Keeps a list of all groups LEDs from current profile.
	const vector<uint8_t*> allLeds;

	mutable vector<uint8_t> cachedBuffer;

	void calculate() override;

};

} // namespace
