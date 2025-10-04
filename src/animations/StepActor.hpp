/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      StepActor.hpp
 * @since     Jul 23, 2019
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

#include "DirectionActor.hpp"

#pragma once

namespace LEDSpicer::Animations {

/**
 * LEDSpicer::StepActor
 *
 * Step actors adds helper methods to create smooth transitions between frames.
 */
class StepActor: public DirectionActor {

public:

	using DirectionActor::DirectionActor;

	virtual ~StepActor() = default;

	/**
	 * Calculates the percent per step to use.
	 * @param totalStepFrames
	 */
	void calculateStepPercent();

protected:

	/// Keeps the pre-calculated %.
	float stepPercent = 0;

	/**
	 * Fades the element at index.
	 * @param index
	 * @param color
	 * @param direction
	 */
	void changeFrameElement(uint16_t index, const Color& color, Directions direction);

	/**
	 * Fades the current element.
	 * @param color
	 * @param direction
	 */
	void changeFrameElement(const Color& color, Directions direction);

	/**
	 * Changes the current element and fade the next one based on time.
	 * @param color
	 * @param fade
	 * @param direction
	 */
	void changeFrameElement(const Color& color, bool fade, Directions direction);

	/**
	 * Changes the current element with a transition between color and colorNext.
	 * @param color
	 * @param colorNext
	 * @param direction
	 */
	void changeFrameElement(const Color& color, const Color& colorNext, Directions direction);

private:

	struct FrameTransition {
		float    percent;
		uint16_t next;
	};

	StepActor::FrameTransition changeFrameElementCommon(const Directions& direction) const;
};

} // namespace
