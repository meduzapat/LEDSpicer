/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Curtain.hpp
 * @since     Sep 25, 2025
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

#include "ActorDriven.hpp"
#include "utilities/Color.hpp"

#pragma once

namespace LEDSpicer::Devices::Transitions {

/**
 * LEDSpicer::Devices::Transitions::Curtain
 * A transition that Covers the current profile and uncovers the target profile.
 */
class Curtain : public ActorDriven {

public:

	enum class ClosingWays : uint8_t {Left, Right, Both};

	using ActorDriven::ActorDriven;

	virtual ~Curtain() = default;

	/**
	 * @see ActorDriven::run()
	 */
	bool run() override;

	/**
	 * @see Transition::drawConfig()
	 */
	void drawConfig() const override;

	/**
	 * Converts a direction into string.
	 * @param direction
	 * @return
	 */
	static string closingWays2str(const ClosingWays closingWays);

	/**
	 * Converts a string into a direction.
	 * @param direction
	 * @return
	 */
	static ClosingWays str2ClosingWays(const string& closingWays);

};

} // namespace
