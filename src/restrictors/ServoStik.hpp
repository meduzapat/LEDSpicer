/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      ServoStik.hpp
 * @since     Jul 9, 2020
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

#include "RestrictorUSB.hpp"
#include <fstream>  // to store and read files.
#include <sys/stat.h>

#ifndef RESTRICTORS_SERVOSTIK_HPP_
#define RESTRICTORS_SERVOSTIK_HPP_ 1

#define SERVOSTIK_NAME       "ServoStik"
#define SERVOSTIK_FULLNAME   "Ultimarc ServoStik"
#define SERVOSTIK_PRODUCT    0x1700
#define SERVOSTIK_INTERFACE  0
#define SERVOSTIK_WVALUE     0x0200
#define SERVOSTIK_MAX_BOARDS 4
#define SERVOSTIK_DATA(name) "/servostik_" + name + ".dat"

namespace LEDSpicer::Restrictors {

/**
 * LEDSpicer::Restrictor::ServoStick
 */
class ServoStik : public RestrictorUSB {

public:

	ServoStik(umap<string, string>& options, umap<string, uint8_t>& playerData) :
	RestrictorUSB(
		playerData,
		SERVOSTIK_WVALUE,
		SERVOSTIK_INTERFACE,
		Utility::parseNumber(options["boardId"], "Invalid Board ID"),
		SERVOSTIK_MAX_BOARDS,
		SERVOSTIK_FULLNAME
	) {}

	virtual ~ServoStik() = default;

	virtual void rotate(const umap<string, Ways>& playersData);

	uint16_t getVendor() const override;

	uint16_t getProduct() const override;

	uint8_t getMaxIds() const override;

	const bool isNonBasedId() const override;

protected:

	void storeWays(const string& profile, const Ways& ways) const;

	const Ways retrieveWays(const string& profile) const;

};

} /* namespace */

#endif /* RESTRICTORS_SERVOSTIK_HPP_ */
