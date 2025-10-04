/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      RestrictorUSB.hpp
 * @since     Jul 7, 2020
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

#include "Restrictor.hpp"
#include "utilities/USB.hpp"

#pragma once

namespace LEDSpicer::Restrictors {

using LEDSpicer::Utilities::USB;

/**
 * LEDSpicer::Restrictor::RestrictorUSB
 * Class to handle restrictor for joysticks connected to USB.
 */
class RestrictorUSB : public USB, public Restrictor {

public:

	RestrictorUSB(
		Uint8UMap&    playerData,
		uint16_t      wValue,
		uint8_t       interface,
		uint8_t       boardId,
		uint8_t       maxBoards,
		const string& name
	) :
		USB(wValue, interface, boardId, maxBoards),
		Restrictor(playerData, name) {}

	virtual ~RestrictorUSB() = default;

	string getFullName() const override;

protected:

	void openHardware() override;

	void closeHardware() override;

	void afterConnect() override {}

	virtual void afterClaimInterface() override {};

};

} // namespace
