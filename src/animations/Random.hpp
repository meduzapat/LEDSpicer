/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Random.hpp
 * @since     Jul 5, 2018
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2018 - 2019 Patricio A. Rossi (MeduZa)
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

#include "utility/Colors.hpp"
#include "FrameActor.hpp"

#ifndef RANDOM_HPP_
#define RANDOM_HPP_ 1

#define REQUIRED_PARAM_ACTOR_RANDOM {"speed", "colors"}

namespace LEDSpicer {
namespace Animations {

/**
 * LEDSpicer::Animations::Random
 */
class Random: public FrameActor, public Colors {

public:

	Random(umap<string, string>& parameters, Group* const group);

	virtual ~Random() = default;

	/**
	 * @see Actor::drawConfig()
	 */
	void drawConfig();

protected:

	vector<const Color*>
		newColors,
		oldColors;

	const vector<bool> calculateElements();

	/**
	 * Picks new colors.
	 */
	void generateNewColors();
};

} /* namespace Animations */
} /* namespace LEDSpicer */

actorFactory(LEDSpicer::Animations::Random)

#endif /* RANDOM_HPP_ */
