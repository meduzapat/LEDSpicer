/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DirectionActor.hpp
 * @since     Jul 18, 2019
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

#include "utility/Direction.hpp"
#include "FrameActor.hpp"

#include <string>

using std::string;

#ifndef DIRECTIONACTOR_HPP_
#define DIRECTIONACTOR_HPP_ 1

namespace LEDSpicer {
namespace Animations {

/**
 * LEDSpicer::DirectionActor
 */
class DirectionActor : public FrameActor, public Direction {

public:

	DirectionActor(
		umap<string, string>& parameters,
		Group* const group,
		const vector<string>& requiredParameters
	);

	virtual ~DirectionActor() = default;

	/**
	 * Draws the actor configuration.
	 */
	virtual void drawConfig();

	/**
	 * @return true if the animation is on the bouncing period.
	 */
	bool isBouncing() const;

	/**
	 * @return true if the animation is a bouncer.
	 */
	bool isBouncer() const;

	/**
	 * @return true if the current frame is the first frame.
	 */
	virtual bool isFirstFrame() const;

	/**
	 * @return true if the current frame is the last frame,
	 */
	virtual bool isLastFrame() const;

	/**
	 * @return true if the direction is forward or forward with bouncing.
	 */
	virtual bool isDirectionForward() const;

	/**
	 * @return true if the direction is backward or backward with bouncing.
	 */
	virtual bool isDirectionBackward() const;

	/**
	 * @see Actor::restart()
	 */
	virtual void restart();

	/**
	 * Will calculate the next element index for a group of element, based on the direction and the current position.
	 * @param [out] currentDirection will be updated with the new direction.
	 * @param element the element index from 1.
	 * @param direction
	 * @param totalElements
	 * @return the next element.
	 */
	static uint8_t calculateNextOf(Directions& currentDirection, uint8_t element, Directions direction, uint8_t totalElements);

protected:

	/// Current Direction
	Directions cDirection;

	/**
	 * Moves the frame to the next one.
	 */
	virtual void advanceFrame();

};

}} /* namespace LEDSpicer */

#endif /* DIRECTIONACTOR_HPP_ */
