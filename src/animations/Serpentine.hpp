/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Serpentine.hpp
 * @since     Jun 22, 2018
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

#include "utility/Color.hpp"
#include "Actor.hpp"
#include "StepActor.hpp"

#ifndef SERPENTINE_HPP_
#define SERPENTINE_HPP_ 1

#define REQUIRED_PARAM_ACTOR_SERPENTINE {"speed", "direction"}

namespace LEDSpicer::Animations {

/**
 * LEDSpicer::Serpentine
 */
class Serpentine : public StepActor, public Color {

public:

	Serpentine(umap<string, string>& parameters, Group* const layout);

	virtual ~Serpentine() = default;

	void drawConfig() const override;

protected:

	void calculateElements() override;

private:

	const Color& tailColor;

	struct TailData {
		uint8_t  percent;
		uint16_t position;
	};

	vector<TailData> tailData;

	void calculateTailPosition();
};

} /* namespace */

#endif /* SERPENTINE_HPP_ */
