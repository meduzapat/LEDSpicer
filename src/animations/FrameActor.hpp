/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      FrameActor.hpp
 * @since     Jul 18, 2019
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

#include "Actor.hpp"
#include "utility/Speed.hpp"

#ifndef FRAMEACTOR_HPP_
#define FRAMEACTOR_HPP_ 1

namespace LEDSpicer::Animations {

/**
 * LEDSpicer::FrameActor
 */
class FrameActor : public Actor, public Speed {

public:

	FrameActor(
		umap<string, string>& parameters,
		Group* const group,
		const vector<string>& requiredParameters
	);

	virtual ~FrameActor() = default;

	/**
	 * @see Actor::drawConfig()
	 */
	void drawConfig() override;

	/**
	 * @return true if the current frame is the first frame.
	 */
	virtual bool isFirstFrame() const;

	/**
	 * @return true if the current frame is the last frame,
	 */
	virtual bool isLastFrame() const;

	/**
	 * @see Actor::draw()
	 */
	void draw() override;

	void restart() override;

	bool isRunning() override;

	bool acceptCycles() override {
		return true;
	}

	/**
	 * Change the start cycles.
	 * @param cycles
	 */
	virtual void setStartCycles(uint8_t cycles);

	/**
	 * Change the end cycles.
	 * @param cycles
	 */
	virtual void setEndCycles(uint8_t seconds);

	const uint16_t getFullFrames() const override;

	const float getRunTime() const override;

protected:

	uint8_t
		/// Total number of frames.
		totalFrames  = 0,
		/// The current frame.
		currentFrame = 0,
		/// The percent of the animation to start.
		startAt      = 0,
		/// Number of cycles to run.
		cycles       = 0,
		/// Current cycles.
		cycle        = 0;

	/**
	 * Advances the system frame forward.
	 */
	virtual void advanceFrame();

};

} /* namespace */

#endif /* FRAMEACTOR_HPP_ */
