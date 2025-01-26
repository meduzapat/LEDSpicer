/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Pulse.hpp
 * @since     Jun 24, 2018
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

#include "DirectionActor.hpp"
#include "utility/Colorful.hpp"
#include <cmath>

#ifndef PULSE_HPP_
#define PULSE_HPP_ 1

#define REQUIRED_PARAM_ACTOR_PULSE {"speed", "direction"}

namespace LEDSpicer::Animations {

/**
 * LEDSpicer::Animations::Pulse
 */
class Pulse : public DirectionActor, public Colorful {

public:

	enum class Modes : uint8_t {Linear, Exponential};

	Pulse(umap<string, string>& parameters, Group* const layout):
		DirectionActor(parameters, layout, REQUIRED_PARAM_ACTOR_PULSE),
		Colorful(parameters.count("color") ? parameters["color"] : parameters.count("colors") ? parameters["colors"] : ""),
		mode(parameters.count("mode") ? str2mode(parameters["mode"]) : Modes::Exponential)
	{}

	virtual ~Pulse() = default;

	void drawConfig() const override;

	static string mode2str(const Modes type);

	static Modes str2mode(const string& type);

	void restart() override;

protected:

	void calculateElements() override;

private:

	const Modes mode;

};

} /* namespace */

#endif /* PULSE_HPP_ */
