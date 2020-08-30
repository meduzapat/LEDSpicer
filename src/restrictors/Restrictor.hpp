/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Restrictor.hpp
 * @since     Jul 7, 2020
 * @author    Patricio A. Rossi (MeduZa)
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

#include "utility/USB.hpp"

#ifndef RESTRICTORS_ROTATOR_HPP_
#define RESTRICTORS_ROTATOR_HPP_ 1

#define REQUIRED_PARAM_RESTRICTOR {"name", "boardId"}
#define REQUIRED_PARAM_MAP        {"player", "joystick", "id"}

namespace LEDSpicer {
namespace Restrictors {

/**
 * LEDSpicer::Restrictor::Restrictor
 * Class to handle restrictor for joysticks.
 */
class Restrictor : public USB {

public:

	enum class Ways : uint8_t {invalid, w2, w2v, w4, w4x, w8, w16, w49, analog, mouse, rotary8, rotary12};

	Restrictor(
		umap<string, string>& options,
		umap<string, uint8_t>& playerData,
		uint16_t wValue,
		uint8_t  interface,
		uint8_t  boardId,
		uint8_t  maxBoards
	) : players(playerData), USB(wValue, interface, boardId, maxBoards) {}

	virtual ~Restrictor() {}

	void initialize();

	void terminate();

	virtual string getName() const = 0;

	/**
	 * @return the number of players a hardware supports.
	 */
	virtual uint8_t getMaxIds() const;

	/**
	 * Executes the rotation.
	 * @param playersData
	 */
	virtual void rotate(const umap<string, Ways>& playersData) = 0;

	/**
	 * Rotate all players to this profile.
	 * @param ways
	 */
	virtual void rotate(Ways ways);

	/**
	 * convert strings into joystick positions.
	 * @param ways
	 * @return defaults to 8 ways.
	 */
	static Ways str2ways(const string& ways);

	static string ways2str(Ways ways);

	/**
	 * @param ways
	 * @return Returns true if the hardware is rotary, false if is restrictor.
	 */
	static bool isRotary(const Ways& ways);

	Ways getWay(const umap<string, Ways>& playersData, bool rotary) const;

protected:

	/// Supported players for this restrictor in the form of P_J => id (player_joystick)
	umap<string, uint8_t> players;

	virtual void disconnect();

	virtual void afterConnect() {}

	virtual void afterClaimInterface() {};

};

} /* namespace Restrictors */
} /* namespace LEDSpicer */

#endif /* RESTRICTORS_ROTATOR_HPP_ */
