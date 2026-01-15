/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      RestrictorSerial.hpp
 * @since     Oct 8, 2023
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

#include "Restrictor.hpp"
#include "utilities/Serial.hpp"

#pragma once

namespace LEDSpicer::Restrictors {

using LEDSpicer::Utilities::Serial;

/**
 * LEDSpicer::Restrictors::RestrictorSerial
 * Class to handle restrictor for joysticks connected to the serial bus.
 */
class RestrictorSerial: public Serial, public Restrictor {

public:

	RestrictorSerial(
		const string& port,
		Uint8UMap&    playerData,
		const string& name
	) :
		Serial(port),
		Restrictor(playerData, name) {}

	virtual ~RestrictorSerial() = default;

	string getFullName() const override;

protected:

	void openHardware() override;

	void closeHardware() override;

};

} // namespace

