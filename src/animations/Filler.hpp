/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Filler.hpp
 * @since     Oct 26, 2018
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

#include "StepActor.hpp"
#include "utilities/Colorful.hpp"

#pragma once

#define REQUIRED_PARAM_ACTOR_FILLER {"speed", "direction", "mode"}

namespace LEDSpicer::Animations {

using namespace LEDSpicer::Utilities;

/**
 * LEDSpicer::Animations::Filler
 */
class Filler: public StepActor, public Colorful {

public:

	enum class Modes : uint8_t {Normal, Random, Wave, Curtain};

	Filler(StringUMap& parameters, Group* const group);

	virtual ~Filler() = default;

	void drawConfig() const override;

	static string mode2str(const Modes mode);

	static Modes str2mode(const string& mode);

	void restart() override;

protected:

	void calculateElements() override;

private:

	/// Stores the mode.
	Modes mode;

	/// Keeps the currently processed random.
	uint16_t currentRandom;

	/// Keeps track of the previous frame affected elements.
	vector<uint8_t> previousFrameAffectedElements;

	/**
	 * Generates the next random element.
	 */
	void generateNextRandom();

	/**
	 * Fills the group in a linear way.
	 */
	void fillElementsLinear();

	/**
	 * Fills the group in a random way.
	 */
	void fillElementsRandom();

	/**
	 * Fills the group like linear but doing waves.
	 * The idea in this mode is to duplicate the number of frames to create a filling and draining effect
	 * in the same direction.
	 */
	void fillElementsWave();

	/**
	 * Fills the group like a curtain, from both sides toward center (inward/forward)
	 * or from center toward both sides (outward/backward).
	 */
	void fillElementsCurtain();
};

} // namespace
