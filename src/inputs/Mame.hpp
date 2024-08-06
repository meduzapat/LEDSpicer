/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Mame.hpp
 * @since     May 12, 2019
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

#include "utility/Socks.hpp"
using LEDSpicer::Socks;
#include "Input.hpp"

#ifndef MAME_HPP_
#define MAME_HPP_ 1

#define MAME_PORT "8000"

namespace LEDSpicer::Inputs {

/**
 * LEDSpicer::Inputs::Mame
 * This input plugin connects to MAME output network port 8000 and read the output states.
 */
class Mame: public Input {

public:

	using Input::Input;

	virtual ~Mame() = default;

	void drawConfig() const override;

	void process() override;

	void activate() override;

	void deactivate() override;

protected:

	Socks socks;

	bool active = true;
};
} /* namespace */

#endif /* MAME_HPP_ */
