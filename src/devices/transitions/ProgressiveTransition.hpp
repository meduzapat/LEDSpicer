/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      ProgressiveTransition.hpp
 * @since     Sep 21, 2025
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

#include "Transition.hpp"
#include "utilities/Speed.hpp"
#include "utilities/Utility.hpp"

#pragma once

namespace LEDSpicer::Devices::Transitions {

/**
 * LEDSpicer::Devices::Transitions::ProgressiveTransition
 * Base class for transitions that use a progress value.
 */
class ProgressiveTransition : public Transition, public Speed {

public:

	ProgressiveTransition() = delete;

	/**
	 * @param current
	 * @param to
	 * @param speed
	 */
	ProgressiveTransition(Profile* current, const string& speed) :
		Transition(current),
		Speed(speed),
		increase(calculateSpeed(this->speed) / (to ? 2 : 1))
	{}

	virtual ~ProgressiveTransition() = default;

	/**
	 * @see Transition::run()
	 */
	bool run() override;

	/**
	 * @see Transition::drawConfig()
	 */
	void drawConfig() const override;

protected:

	/// Keeps the progress value from 0 to 100.
	float progress = 0;

	/// Stores the increase per call.
	const float increase;

	/**
	 * Calculates the next progress.
	 */
	virtual void calculate() = 0;

	/**
	 * @param The speed to calculate.
	 * @return A value for time based on speed and FPS
	 */
	static float calculateSpeed(const Speeds speed);

	/**
	 * @see Transition::reset()
	 */
	void reset() override;
};

} // namespace
