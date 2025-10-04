/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Network.hpp
 * @since     Jul 24, 2020
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

#include "Input.hpp"
#include "utilities/Socks.hpp"

#pragma once

#define DEFAULT_PORT "16162"

namespace LEDSpicer::Inputs {

/**
 * LEDSpicer::Inputs::Network
 */
class Network: public Input {

public:

	Network(StringUMap& parameters, ItemPtrUMap& inputMaps) :
		Input(parameters, inputMaps) {}

	virtual ~Network() = default;

	void activate() override;

	void deactivate() override;

	void process() override;

protected:

	static Socks sock;
};

} // namespace
