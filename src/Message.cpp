/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Message.cpp
 * @since     Nov 18, 2018
 * @author    Patricio A. Rossi (MeduZa)
 * @copyright Copyright © 2018 - 2019 Patricio A. Rossi (MeduZa)
 */

#include "Message.hpp"

using namespace LEDSpicer;

string Message::toString() {
	string ret;
	for (auto& s : data)
		ret.append(s).push_back(DELIMITER);
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
