/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      StepActor.hpp
 * @since     Jul 23, 2019
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2018 - 2024 Patricio A. Rossi (MeduZa)
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

#ifndef STEPACTOR_HPP_
#define STEPACTOR_HPP_ 1

#include "DirectionActor.hpp"

namespace LEDSpicer::Animations {

/**
 * LEDSpicer::StepActor
 */
class StepActor: public DirectionActor {

public:

	StepActor(
		umap<string, string>& parameters,
		Group* const group,
		const vector<string>& requiredParameters
	);

	virtual ~StepActor() = default;

	/**
	 * @see Actor::drawConfig()
	 */
	virtual void drawConfig();

	/**
	 * @return The number of steps for the whole animation.
	 */
	uint16_t getTotalSteps() const;

	/**
	 * @return The current relative step.
	 */
	uint16_t getCurrentStep() const;

	/**
	 * @see Actor::restart()
	 */
	virtual void restart();

	/**
	 * @see DirectionActor::isFirstFrame()
	 */
	virtual bool isFirstFrame() const;

	/**
	 * @see DirectionActor::isLastFrame()
	 */
	virtual bool isLastFrame() const;

	/**
	 * Sets the total step frames.
	 * @param totalStepFrames
	 */
	void setTotalStepFrames(uint8_t totalStepFrames);

protected:

	uint8_t
		totalStepFrames  = 0,
		currentStepFrame = 0;

	/// Keeps the pre-calculated 1%.
	float stepPercent = 0;

	virtual void advanceFrame();

	/**
	 * Fades the element at index.
	 * @param index
	 * @param color
	 * @param direction
	 */
	void changeFrameElement(uint8_t index, const Color& color, Directions direction);

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

};

} /* namespace */

#endif /* STEPACTOR_HPP_ */
