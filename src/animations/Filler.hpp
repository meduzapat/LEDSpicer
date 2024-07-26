/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Filler.hpp
 * @since     Oct 26, 2018
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

#include "StepActor.hpp"
#include "utility/Colorful.hpp"

#ifndef FILLER_HPP_
#define FILLER_HPP_ 1

#define REQUIRED_PARAM_ACTOR_FILLER {"speed", "direction", "mode"}

namespace LEDSpicer::Animations {

/**
 * LEDSpicer::Animations::Fill
 */
class Filler: public StepActor, public Colorful {

public:

	enum class Modes : uint8_t {Normal, Random};

	Filler(umap<string, string>& parameters, Group* const group);

	virtual ~Filler() = default;

	void drawConfig() override;

	string mode2str(Modes mode);

	Modes str2mode(const string& mode);

	void restart() override;

protected:

	struct Data {
		uint8_t
			begin,
			end;
		Directions dir;
	};

	void calculateElements() override;

private:

	/// Stores the mode.
	Modes mode;

	/// Keeps the currently processed random.
	uint8_t currentRandom;

	/// Keeps track if the process is filling / emptying.
	bool filling = true;

	/// Keeps track of the previous frame affected elements.
	vector<bool> previousFrameAffectedElements;

	/**
	 * Generates the next random element.
	 */
	void generateNextRandom();

	/**
	 * Fills the group in a linear way.
	 * @param values
	 */
	void fillElementsLinear(const Data& values);

	/**
	 * Fills the group in a random way.
	 */
	void fillElementsRandom();

};

} /* namespace */

#endif /* FILLER_HPP_ */
