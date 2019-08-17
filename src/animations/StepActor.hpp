/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      StepActor.hpp
 * @since     Jul 23, 2019
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2019 Patricio A. Rossi (MeduZa)
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

namespace LEDSpicer {
namespace Animations {

/**
 * LEDSpicer::StepActor
 */
class StepActor: public Animations::DirectionActor {

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

protected:

	uint8_t
		totalStepFrames  = 0,
		currentStepFrame = 0;

	virtual void advanceFrame();

	/**
	 * Changes the current element and fade the next one based on time.
	 * @param color
	 * @param fade
	 */
	void changeFrameElement(const Color& color, bool fade = false);

	/**
	 * Changes the current element with a transition between color and colorNext.
	 * @param color
	 * @param colorNext
	 */
	void changeFrameElement(const Color& color, const Color& colorNext);

};

}} /* namespace LEDSpicer */

#endif /* STEPACTOR_HPP_ */
