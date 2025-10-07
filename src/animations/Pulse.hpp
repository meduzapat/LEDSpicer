/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Pulse.hpp
 * @since     Jun 24, 2018
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2018 - 2026 Patricio A. Rossi (MeduZa)
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
#include "utilities/Colorful.hpp"

#pragma once

#define REQUIRED_PARAM_ACTOR_PULSE {"speed", "direction"}

namespace LEDSpicer::Animations {

/**
 * LEDSpicer::Animations::Pulse
 */
class Pulse : public DirectionActor, public Colorful {

public:

	enum class Modes : uint8_t {Linear, Exponential};

	Pulse(StringUMap& parameters, Group* const layout);

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

} // namespace

