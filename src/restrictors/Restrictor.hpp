/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Restrictor.hpp
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

#include "utility/Hardware.hpp"

#ifndef RESTRICTORS_ROTATOR_HPP_
#define RESTRICTORS_ROTATOR_HPP_ 1

#define REQUIRED_PARAM_RESTRICTOR {"name"}
#define REQUIRED_PARAM_MAP        {"player", "joystick"}
#define RESTRICTOR_SINGLE_ID      1

namespace LEDSpicer::Restrictors {

/**
 * LEDSpicer::Restrictor::Restrictor
 * Class to handle restrictor for joysticks.
 */
class Restrictor : public Hardware {

public:

	enum class Ways : uint8_t {invalid, w2, w2v, w4, w4x, w8, w16, w49, analog, mouse, rotary8, rotary12};

	Restrictor(
		umap<string, uint8_t>& playerData,
		const string& name
	) : Hardware(name), players(playerData) {}

	virtual ~Restrictor() = default;

	void initialize();

	void terminate();

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
	 * Convert human readable strings into joystick positions.
	 * @param ways
	 * @return defaults to 8 ways.
	 */
	static Ways str2ways(const string& ways);

	/**
	 * Convert Ways into human readable strings.
	 * @param ways
	 * @return
	 */
	static string ways2str(Ways ways);

	/**
	 * Convert a string representation of a ways into ways.
	 * @param strWays
	 * @return
	 */
	static const Ways strWays2Ways(const std::string& strWays);

	/**
	 * Converts ways into its string representation.
	 * @param ways
	 * @return
	 */
	static const string ways2StrWays(Ways ways);

	/**
	 * @param ways
	 * @return Returns true if the hardware is rotary, false if is restrictor.
	 */
	static bool isRotary(const Ways& ways);

	/**
	 * Converts a profile into human readable.
	 * @param profile like 1_1
	 * @return a string like Player 1, Joystick 1
	 */
	static const string getProfileStr(const string& profile);

protected:

	/// Supported players for this restrictor in the form of P_J => id (player_joystick)
	umap<string, uint8_t> players;

};

} /* namespace */

#endif /* RESTRICTORS_ROTATOR_HPP_ */
