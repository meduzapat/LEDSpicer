/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Message.hpp
 * @since     Jul 9, 2018
 * @author    Patricio A. Rossi (MeduZa)
 * @copyright Copyright © 2018 - 2019 Patricio A. Rossi (MeduZa)
 */

#include <string>
using std::string;

#include <vector>
using std::vector;

#include "utility/Utility.hpp"

#ifndef MESSAGE_HPP_
#define MESSAGE_HPP_ 1

#define DELIMITER '|'

namespace LEDSpicer {

struct Message {

	enum class Types : uint8_t {
		Invalid,
		LoadProfile,
		LoadProfileByEmulator,
		FinishLastProfile,
		SetElement,
		ClearElement,
		ClearAllElements,
		SetGroup,
		ClearGroup,
		ClearAllGroups
	};

	string toString();

	static string type2str(Types type);

	static Types str2type(const string& type);

	const vector<string>& getData() const;

	void setData(const vector<string>& data);

	void addData(const string& data);

	Types getType() const;

	void setType(Types type);

protected:

	Types type = Types::Invalid;

	vector<string> data;
};

}
#endif /* MESSAGE_HPP_ */
