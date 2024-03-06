/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Message.cpp
 * @since     Nov 18, 2018
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

#include "Message.hpp"

using namespace LEDSpicer;

string Message::toString() {
	string ret;
	for (auto& s : data)
		ret.append(s).push_back(RECORD_SEPARATOR);
	ret.append(to_string(flags)).push_back(RECORD_SEPARATOR);
	ret.append(to_string(static_cast<uint8_t>(type))).push_back('\0');
	return ret;
}

string Message::toHumanString() {
	return Utility::implode(data, ID_GROUP_SEPARATOR);
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

string Message::flag2str(uint8_t flags) {
	vector<string> result;
	if (flags & FLAG_NO_ANIMATIONS)
		result.push_back("NO_ANIMATIONS");
	if (flags & FLAG_NO_INPUTS)
		result.push_back("NO_INPUTS");
	if (flags & FLAG_NO_START_TRANSITIONS)
		result.push_back("NO_START_TRANSITIONS");
	if (flags & FLAG_NO_END_TRANSITIONS)
		result.push_back("NO_END_TRANSITIONS");
	if (flags & FLAG_NO_ROTATOR)
		result.push_back("NO_ROTATOR");
	if (flags & FLAG_FORCE_RELOAD)
		result.push_back("FORCE_RELOAD");
	if (flags & FLAG_REPLACE)
		result.push_back("REPLACE");
	return Utility::implode(result, ' ');
}

void Message::str2flag(uint8_t& currentFlags, const string& flags) {
	auto parts = Utility::explode(flags, '|');
	bool rotator = false;
	for (auto& flag : parts) {
		if (flag == "NO_ANIMATIONS")
			currentFlags |= FLAG_NO_ANIMATIONS;
		else if (flag == "NO_INPUTS")
			currentFlags |= FLAG_NO_INPUTS;
		else if (flag == "NO_START_TRANSITIONS")
			currentFlags |= FLAG_NO_START_TRANSITIONS;
		else if (flag == "NO_END_TRANSITIONS")
			currentFlags |= FLAG_NO_END_TRANSITIONS;
		else if (flag == "NO_TRANSITIONS")
			currentFlags |= FLAG_NO_START_TRANSITIONS | FLAG_NO_END_TRANSITIONS;
		else if (flag == "NO_ROTATOR")
			currentFlags |= FLAG_NO_ROTATOR;
		else if (flag == "SHOW_ROTATOR")
			rotator = true;
		else if (flag == "FORCE_RELOAD")
			currentFlags |= FLAG_FORCE_RELOAD;
		else if (flag == "REPLACE")
			currentFlags |= FLAG_REPLACE;
	}
	if (rotator)
		currentFlags &= ~FLAG_NO_ROTATOR;
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

const uint8_t Message::getFlags() const {
	return flags;
}

void Message::setFlags(const uint8_t flags) {
	this->flags = flags;
}

void Message::reset() {
	type = Types::Invalid;
	data.clear();
}
