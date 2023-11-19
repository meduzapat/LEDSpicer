/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Message.hpp
 * @since     Jul 9, 2018
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2018 - 2020 Patricio A. Rossi (MeduZa)
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

	string toString();

	static string type2str(Types type);

	static Types str2type(const string& type);

	const vector<string>& getData() const;

	void setData(const vector<string>& data);

	void addData(const string& data);

	Types getType() const;

	void setType(Types type);

	void reset();

protected:

	Types type = Types::Invalid;

	vector<string> data;
};

}
#endif /* MESSAGE_HPP_ */
