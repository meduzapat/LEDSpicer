/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      FrameActor.hpp
 * @since     Jul 18, 2019
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

#include "Actor.hpp"
#include "utilities/Speed.hpp"

#pragma once

namespace LEDSpicer::Animations {

using LEDSpicer::Utilities::Speed;

/**
 * LEDSpicer::FrameActor
 *
 * Adds Speed and Frames to the Actor class.
 */
class FrameActor : public Actor, public Speed {

public:

	struct Stepping {
		uint16_t
		/// Total number of frames, always bigger than zero and zero based.
		frames = 0,
		/// The current frame (zero based).
		frame  = 0,
		/// Number of system frames to advance an actor frame (handles speed) zero for none.
		steps  = 0,
		/// Current speed step (zero based).
		step   = 0;

		void advanceFrame() {
			++frame;
			if (frame == frames) {
				frame = 0;
			}
		}

		void advanceStep() {
			if (shouldMoveFrame()) advanceFrame();
		}

		bool shouldMoveFrame() {
			if (not steps) return true;
			if (isLastStep()) {
				step = 0;
				return true;
			}
			++step;
			return false;
		}

		uint16_t getLastFrame() const {
			return frames - 1;
		}

		bool isLastFrame() const {
			return frame == getLastFrame();
		}

		bool isLastStep() const {
			// zero steps mean no stepping.
			return not steps or step == steps - 1;
		}
	};

	FrameActor(
		StringUMap& parameters,
		Group* const group,
		const vector<string>& requiredParameters
	);

	virtual ~FrameActor() = default;

	/**
	 * @see Actor::drawConfig()
	 */
	void drawConfig() const override;

	/**
	 * @return true if the current frame is the first frame.
	 */
	virtual bool isFirstFrame() const;

	/**
	 * @return true if the current frame is the first frame including the first step.
	 */
	virtual bool isStartOfCycle() const;

	/**
	 * @return true if the current frame is the last frame including last step (not cycle).
	 */
	virtual bool isLastFrame() const;

	/**
	 * @return true if the current frame is the very last of the cycle (not frame).
	 */
	virtual bool isEndOfCycle() const;

	/**
	 * @see Actor::draw()
	 */
	void draw() override;

	/**
	 * @see Actor::restart();
	 */
	void restart() override;

	bool isRunning() override;

	bool acceptCycles() const override {
		return true;
	}

	uint16_t getFullFrames() const override;

	float getRunTime() const override;

	/**
	 * Calculates the stepping based on the speed.
	 * @param speed
	 * @return the total steps for this speed.
	 */
	static uint16_t calculateStepsBySpeed(Speeds speed);

	/**
	 * Calculates the numbers of frames based on speed.
	 * @param speed
	 * @return the total number of frames based on speed and FPS.
	 */
	static uint16_t calculateFramesBySpeed(Speeds speed);

protected:

	Stepping stepping;

	uint8_t
		/// The percent of the animation to start.
		startAt = 0,
		/// Will run for this number of cycles.
		cycles  = 0,
		/// Current cycle.
		cycle   = 0;

	/**
	 * Advances the system frame forward.
	 */
	virtual void advanceFrame();

};

} // namespace

