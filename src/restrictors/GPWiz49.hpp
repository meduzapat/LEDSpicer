/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      GPWiz49.hpp
 * @since     Aug 11, 2020
 * @author    Patricio A. Rossi (MeduZa) & GPWiz49 related code Chris Newton (mahuti)
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

#include "RestrictorUSB.hpp"

#ifndef RESTRICTORS_GPWIZ49_HPP_
#define RESTRICTORS_GPWIZ49_HPP_ 1

#define GPWIZ49_NAME       "GPWiz49"
#define GPWIZ49_FULLNAME   "GroovyGameGear GP-Wiz49"
#define GPWIZ49_PRODUCT    0x0007
#define GPWIZ49_INTERFACE  0
#define GPWIZ49_WVALUE     0x0200
#define GPWIZ49_MAX_BOARDS 4
#define WILLIAMS "williams"

namespace LEDSpicer::Restrictors {

/**
 * LEDSpicer::Restrictor::GPWiz49
 */
class GPWiz49 : public RestrictorUSB {

public:

	GPWiz49(umap<string, string>& options, umap<string, uint8_t>& playerData) :
	RestrictorUSB(
		playerData,
		GPWIZ49_WVALUE,
		GPWIZ49_INTERFACE,
		Utility::parseNumber(options["boardId"], "Invalid Board ID"),
		GPWIZ49_MAX_BOARDS,
		GPWIZ49_FULLNAME
	),
	williams(options.count(WILLIAMS) ? options.at(WILLIAMS) == "True" : false) {}

	virtual ~GPWiz49() = default;

	virtual void rotate(const umap<string, Ways>& playersData);

	virtual uint16_t getVendor() const;

	virtual uint16_t getProduct() const;

protected:

	// Handles Williams Sinistar mode.
	bool williams = false;
};

} /* namespace */

#endif /* RESTRICTORS_GPWIZ49_HPP_ */
