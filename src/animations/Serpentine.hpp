/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Serpentine.hpp
 * @since     Jun 22, 2018
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

#include "utilities/Color.hpp"
#include "StepActor.hpp"

#pragma once

#define REQUIRED_PARAM_ACTOR_SERPENTINE {"speed", "direction"}

namespace LEDSpicer::Animations {

using namespace LEDSpicer::Utilities;

/**
 * LEDSpicer::Serpentine
 */
class Serpentine : public StepActor, public Color {

public:

	Serpentine(StringUMap& parameters, Group* const layout);

	virtual ~Serpentine() = default;

	void drawConfig() const override;

protected:

	void calculateElements() override;

private:

	const Color& tailColor;

	struct TailData {
		uint8_t  percent  = 0;
		uint16_t position = 0;
	};

	vector<TailData> tailData;

	void calculateTailPosition();
};

} // namespace
