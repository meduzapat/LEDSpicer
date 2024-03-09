/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Message.hpp
 * @since     Jul 9, 2018
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

#include <string>
using std::string;

#include <vector>
using std::vector;

#include "utility/Utility.hpp"

#ifndef MESSAGE_HPP_
#define MESSAGE_HPP_ 1

namespace LEDSpicer {

struct Message {

	enum class Types : uint8_t {
		Invalid,
		LoadProfile,
		LoadProfileByEmulator,
		FinishLastProfile,
		FinishAllProfiles,
		SetElement,
		ClearElement,
		ClearAllElements,
		SetGroup,
		ClearGroup,
		ClearAllGroups,
		CraftProfile
	};

	Message() = default;

	Message(Types type) : type(type) {}

	/**
	 * Creates a string to be send over the wire.
	 * @return a string with the data.
	 */
	string toString();

	/**
	 * Creates a string to be displayed.
	 * @return a string with the data in human readable format.
	 */
	string toHumanString();

	/**
	 * Convert the type to string.
	 * @param type
	 * @return
	 */
	static string type2str(Types type);

	/**
	 * Convert the strin to type.
	 * @param type
	 * @return
	 */
	static Types str2type(const string& type);

	/**
	 * Converts the flags into strings to be send over the wire.
	 * @param flags
	 * @return
	 */
	static string flag2str(uint8_t flags);

	/**
	 * Extract the flags from a string.
	 * @param currentFlags
	 * @param flags
	 */
	static void str2flag(uint8_t& currentFlags, const string& flags);

	const vector<string>& getData() const;

	void setData(const vector<string>& data);

	void addData(const string& data);

	Types getType() const;

	void setType(Types type);

	const uint8_t getFlags() const;

	void setFlags(const uint8_t flags);

	void reset();

protected:

	Types type = Types::Invalid;

	uint8_t flags = 0;

	vector<string> data;
};

}
#endif /* MESSAGE_HPP_ */
