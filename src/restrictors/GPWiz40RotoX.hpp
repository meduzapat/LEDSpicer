/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      GPWiz40RotoX.hpp
 * @since     Aug 11, 2020
 * @author    Patricio A. Rossi (MeduZa) & GPWiz40RotoX related code Chris Newton (mahuti)
 *
 * @copyright Copyright © 2020 Patricio A. Rossi (MeduZa)
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
#include "Brands.hpp"

#ifndef RESTRICTORS_GPWIZ40ROTOX_HPP_
#define RESTRICTORS_GPWIZ40ROTOX_HPP_ 1

#define GPWIZ40ROTOX_NAME       "GPWiz40 + Roto-X"
#define GPWIZ40ROTOX_PRODUCT    0x0035
#define GPWIZ40ROTOX_INTERFACE  0
#define GPWIZ40ROTOX_WVALUE     0x0200
#define GPWIZ40ROTOX_MAX_BOARDS 4

namespace LEDSpicer {
namespace Restrictors {

/**
 * LEDSpicer::Restrictor::GPWiz40RotoX
 */
class GPWiz40RotoX : public Restrictor {

public:

	GPWiz40RotoX(umap<string, string>& options) :
		Restrictor(
			options,
			GPWIZ40ROTOX_WVALUE,
			GPWIZ40ROTOX_INTERFACE,
			Utility::parseNumber(options["boardId"], "Invalid Board ID"),
			GPWIZ40ROTOX_MAX_BOARDS
		) {}


	virtual ~GPWiz40RotoX() = default;

	virtual void rotate(Ways way);

	virtual uint16_t getVendor();

	virtual uint16_t getProduct();

	virtual string getName();
};

} /* namespace Rotators */
} /* namespace LEDSpicer */

#endif /* RESTRICTORS_GPWIZ40ROTOX_HPP_ */
