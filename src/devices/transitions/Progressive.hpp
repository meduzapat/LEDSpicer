/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Progressive.hpp
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

#include "Transition.hpp"
#include "utilities/Speed.hpp"

#pragma once

namespace LEDSpicer::Devices::Transitions {

using LEDSpicer::Utilities::Speed;

/**
 * LEDSpicer::Devices::Transitions::Progressive
 * A transition that progresses over time using speed settings.
 */
class Progressive : public Transition, public Speed {

public:

	/**
	 * @param speed The speed setting for the transition.
	 */
	Progressive(const string& speed);

	virtual ~Progressive() = default;

	/**
	 * @see Transition::run()
	 */
	bool run() override;

	/**
	 * @see Transition::drawConfig()
	 */
	void drawConfig() const override;

protected:

	/// Current progress percentage (0-100).
	uint8_t progress = 0;

	/// Step increment per frame.
	uint8_t step = 1;

	/// Progress limit.
	static constexpr float COMPLETE = 99.9f;

	/**
	 * @see Transition::reset()
	 */
	void reset() override;

	/**
	 * Calculates the transition effect for the current progress.
	 * Subclasses must implement this.
	 */
	virtual void calculate() = 0;
};

} // namespace
