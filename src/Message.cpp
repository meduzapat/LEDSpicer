/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Message.cpp
 * @since     Nov 18, 2018
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

#include "Message.hpp"

using namespace LEDSpicer;

string Message::toString() {
	string ret;
	for (auto& s : data)
		ret.append(s).push_back(RECORD_SEPARATOR);
	ret.append(to_string(static_cast<uint8_t>(type))).push_back('\0');
	return ret;
}


string Message::type2str(Types type) {
	switch (type) {
	case Types::LoadProfile:
		return "LoadProfile";
	case Types::LoadProfileByEmulator:
		return "LoadProfileByEmulator";
	case Types::FinishLastProfile:
		return "FinishLastProfile";
	case Types::FinishAllProfiles:
		return "FinishAllProfiles";
	case Types::SetElement:
		return "SetElement";
	case Types::ClearElement:
		return "ClearElement";
	case Types::ClearAllElements:
		return "ClearAllElements";
	case Types::SetGroup:
		return "SetGroup";
	case Types::ClearGroup:
		return "ClearGroup";
	case Types::ClearAllGroups:
		return "ClearAllGroups";
	case Types::CraftProfile:
		return "CraftProfile";
	}
	throw Error("Invalid type");
}

Message::Types Message::str2type(const string& type) {
	if (type == "LoadProfile")
		return Types::LoadProfile;
	if (type == "LoadProfileByEmulator")
		return Types::LoadProfileByEmulator;
	if (type == "FinishLastProfile")
		return Types::FinishLastProfile;
	if (type == "FinishAllProfiles")
		return Types::FinishAllProfiles;
	if (type == "SetElement")
		return Types::SetElement;
	if (type == "ClearElement")
		return Types::ClearElement;
	if (type == "ClearAllElements")
		return Types::ClearAllElements;
	if (type == "SetGroup")
		return Types::SetGroup;
	if (type == "ClearGroup")
		return Types::ClearGroup;
	if (type == "ClearAllGroups")
		return Types::ClearAllGroups;
	if (type == "CraftProfile")
		return Types::CraftProfile;
	throw Error("Invalid type " + type);
}

const vector<string>& LEDSpicer::Message::getData() const {
	return data;
}

void Message::setData(const vector<string>& data) {
	this->data = data;
}

void Message::addData(const string& data) {
	this->data.push_back(data);
}

Message::Types Message::getType() const {
	return type;
}

void Message::setType(Types type) {
	this->type = type;
}

void Message::reset() {
	type = Types::Invalid;
	data.clear();
}
