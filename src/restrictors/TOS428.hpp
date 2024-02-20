/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      TOS428.hpp
 * @since     Oct 8, 2023
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

#include "RestrictorSerial.hpp"

#ifndef RESTRICTORS_TOS428_HPP_
#define RESTRICTORS_TOS428_HPP_ 1

#define TOS428_NAME     "TOS428"
#define TOS428_FULLNAME "GRS TOS Restrictor"
#define TOS428_MAX_ID   4
#define TOS428_PORT     "/dev/ttyACM"
#define TOS428_MAX_PORT 10
#define TOS428_FLAG     "tos428"

#define TOS428_DATA(player, ways) {'s', 'e', 't', 'w', 'a', 'y', ',', player, ',', ways}

namespace LEDSpicer::Restrictors {

/**
 * LEDSpicer::Restrictors::TOS428
 * Implementation for the TOS GRS Gate Restrictor
 */
class TOS428: public RestrictorSerial {

public:

	TOS428(umap<string, string>& options, umap<string, uint8_t>& playerData) :
	RestrictorSerial(
		options.count("port") ? options.at("port") : "",
		playerData,
		TOS428_FULLNAME
	) {}

	virtual ~TOS428() = default;

	virtual void rotate(const umap<string, Ways>& playersData);

	uint8_t getMaxIds() const;

protected:

	virtual void detectPort();

	void createPackage(Ways way, uint8_t target);

	bool sendGreetings();

};

} /* namespace */

#endif /* RESTRICTORS_TOS428_HPP_ */
