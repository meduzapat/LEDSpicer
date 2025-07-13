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
#include "utility/USB.hpp"

#ifndef RESTRICTORS_USB_HPP_
#define RESTRICTORS_USB_HPP_ 1

namespace LEDSpicer::Restrictors {

/**
 * LEDSpicer::Restrictor::RestrictorUSB
 * Class to handle restrictor for joysticks connected to USB.
 */
class RestrictorUSB : public USB, public Restrictor {

public:

	RestrictorUSB(
		umap<string, uint8_t>& playerData,
		uint16_t wValue,
		uint8_t  interface,
		uint8_t  boardId,
		uint8_t  maxBoards,
		const string& name
	) :
		USB(wValue, interface, boardId, maxBoards),
		Restrictor(playerData, name) {}

	virtual ~RestrictorUSB() = default;

	virtual string getFullName() const;

protected:

	virtual void openHardware();

	virtual void closeHardware();

	virtual void afterConnect() {}

	virtual void afterClaimInterface() {};

};

} /* namespace */

#endif /* RESTRICTORS_USB_HPP_ */
