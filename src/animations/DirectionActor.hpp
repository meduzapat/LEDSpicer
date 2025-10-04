/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DirectionActor.hpp
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

#include "utilities/Direction.hpp"
#include "FrameActor.hpp"

#pragma once

namespace LEDSpicer::Animations {

using LEDSpicer::Utilities::Direction;

/**
 * LEDSpicer::DirectionActor
 */
class DirectionActor : public FrameActor, public Direction {

public:

	DirectionActor(
		StringUMap& parameters,
		Group* const group,
		const vector<string>& requiredParameters
	) :
		FrameActor(parameters, group, requiredParameters),
		Direction(parameters["direction"], parameters["bouncer"] == "True"),
		cDirection(direction, false)
	{}

	virtual ~DirectionActor() = default;

	/**
	 * @see FrameActor::draw()
	 */
	void drawConfig() const override;

	/**
	 * @see FrameActor::restart();
	 */
	void restart() override;

	/**
	 * @return true if the animation is on the bouncing period.
	 */
	bool isBouncing() const;

	/**
	 * @return true same as frame actor but includes direction and bouncing effect.
	 */
	bool isFirstFrame() const override;

	/**
	 * @return true same as frame actor but includes direction and bouncing effect.
	 */
	bool isStartOfCycle() const override;

	/**
	 * @return true if the current frame is the last frame including last step (not cycle).
	 */
	bool isLastFrame() const override;

	/**
	 * @return true if the current frame is the very last of the cycle (not frame).
	 */
	bool isEndOfCycle() const override;

	uint16_t getFullFrames() const override;

	/**
	 * Will calculate the next index for a group of items, based on the direction and the current position.
	 * @param [out] currentDirection will be updated with the new direction.
	 * @param index the item index from 0.
	 * @param direction the direction to use.
	 * @param last the last valid item.
	 * @return the next element.
	 */
	static uint16_t calculateNextOf(
		Direction&      currentDirection,
		const uint16_t  index,
		const Direction direction,
		const uint16_t  last
	);

	/**
	 * Will calculate the next index for a group of items, based on the direction and the current position.
	 * But will not change direction.
	 * @param currentDirection
	 * @param index
	 * @param direction
	 * @param last
	 * @return the next element based on direction and item's position.
	 */
	static uint16_t nextOf(
		Direction       currentDirection,
		const uint16_t  index,
		const Direction direction,
		const uint16_t  last
	);

protected:

	/// Dynamic state for current direction and bouncing.
	Direction cDirection;

	/**
	 * Moves the frame to the next one (forward or backward).
	 */
	void advanceFrame() override;

};

} // namespace
