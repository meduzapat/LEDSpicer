/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      GPWiz40RotoX.hpp
 * @since     Aug 11, 2020
 * @author    Patricio A. Rossi (MeduZa) & GPWiz40RotoX related code Chris Newton (mahuti)
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

#include "RestrictorUSB.hpp"

#ifndef RESTRICTORS_GPWIZ40ROTOX_HPP_
#define RESTRICTORS_GPWIZ40ROTOX_HPP_ 1

#define GPWIZ40ROTOX_NAME       "GPWiz40RotoX"
#define GPWIZ40ROTOX_FULLNAME   "GroovyGameGear GP-Wiz40 RotoX"
#define GPWIZ40ROTOX_PRODUCT    0x0035
#define GPWIZ40ROTOX_INTERFACE  0
#define GPWIZ40ROTOX_WVALUE     0x0200
#define GPWIZ40ROTOX_MAX_BOARDS 4
#define GPWIZ40ROTOX_MAX_ID     2

#define SPEED_ON  "speedOn"
#define SPEED_OFF "speedOff"

#define DEFAULT_SPEED 12

namespace LEDSpicer::Restrictors {

/**
 * LEDSpicer::Restrictor::GPWiz40RotoX
 */
class GPWiz40RotoX : public RestrictorUSB {

public:

	GPWiz40RotoX(umap<string, string>& options, umap<string, uint8_t>& playerData) :
	RestrictorUSB(
		playerData,
		GPWIZ40ROTOX_WVALUE,
		GPWIZ40ROTOX_INTERFACE,
		Utility::parseNumber(options["boardId"], "Invalid Board ID"),
		GPWIZ40ROTOX_MAX_BOARDS,
		GPWIZ40ROTOX_FULLNAME
	),
	speedOn(
		options.count(SPEED_ON) ? Utility::parseNumber(
			options.at(SPEED_ON),
			"Invalid value for " SPEED_ON
		)  : DEFAULT_SPEED
	),
	speedOff(
		options.count(SPEED_OFF) ? Utility::parseNumber(
			options.at(SPEED_OFF),
			"Invalid value for " SPEED_OFF
		) : DEFAULT_SPEED
	) {}

	virtual ~GPWiz40RotoX() = default;

	virtual void rotate(const umap<string, Ways>& playersData);

	virtual uint16_t getVendor() const;

	virtual uint16_t getProduct() const;

	virtual uint8_t getMaxIds() const;

protected:

	uint8_t
		/// The length of time the button is depressed and released before sending a signal again.
		speedOn,
		/// The length of time the button is depressed and released before sending a signal again.
		speedOff;

};

} /* namespace */

#endif /* RESTRICTORS_GPWIZ40ROTOX_HPP_ */
