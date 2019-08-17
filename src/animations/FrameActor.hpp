/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      FrameActor.hpp
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

#include "Actor.hpp"
#include "utility/Speed.hpp"

#ifndef FRAMEACTOR_HPP_
#define FRAMEACTOR_HPP_ 1

namespace LEDSpicer {
namespace Animations {

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
	virtual void drawConfig();

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
	virtual bool draw();

protected:

	uint8_t
		totalFrames  = 0,
		currentFrame = 0;

	/**
	 * Advances the system frame forward.
	 */
	virtual void advanceFrame();

};

}} /* namespace LEDSpicer */

#endif /* FRAMEACTOR_HPP_ */
