/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Gradient.hpp
 * @since     Jul 3, 2018
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

#include "utility/Colors.hpp"
#include "StepActor.hpp"

#ifndef GRADIENT_HPP_
#define GRADIENT_HPP_ 1

#define DEFAULT_TONES 10

#define REQUIRED_PARAM_ACTOR_GRADIENT {"speed", "direction", "colors", "mode"}

namespace LEDSpicer::Animations {

/**
 * LEDSpicer::Animations::Rainbow
 */
class Gradient: public StepActor, public Colors {

public:

	/**
	 * All: will gradient every element with the same color.
	 * Cyclic: Will create a smooth gradient over the elements in the group.
	 */
	enum class Modes : uint8_t {All, Cyclic};

	Gradient(umap<string, string>& parameters, Group* const layout);

	virtual ~Gradient() = default;

	void drawConfig() const override;

	static Modes str2mode(const string& mode);

	static string mode2str(const Modes mode);

protected:

	const Modes mode;

	const uint8_t tones;

	vector<Color> precalc;

	void calculateElements() override;

private:

	void calculateSingle();

	void calculateMultiple();

};

} /* namespace */

#endif /* GRADIENT_HPP_ */
