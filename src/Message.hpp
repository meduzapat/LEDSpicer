/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Message.hpp
 * @since     Jul 9, 2018
 * @author    Patricio A. Rossi (MeduZa)
 * @copyright Copyright © 2018 Patricio A. Rossi (MeduZa)
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

	enum class Types : uint8_t {LoadProfile, FinishLastProfile, SetLed, SetElement};

	Types type;

	vector<string> data;

	string toString() {
		string ret;
		for (auto& s : data)
			ret.append(s).push_back(DELIMITER);
		ret.append(to_string(static_cast<uint8_t>(type))).push_back(DELIMITER);
		return ret;
	}
};

}
#endif /* MESSAGE_HPP_ */
