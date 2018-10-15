/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file		Messages.cpp
 * @ingroup
 * @since		Jul 8, 2018
 * @author		Patricio A. Rossi (MeduZa)
 * @copyright	Copyright © 2018 Patricio A. Rossi (MeduZa)
 */

#include "Messages.hpp"

using namespace LEDSpicer;

bool Messages::read() {
	string buffer;
	if (recive(buffer)) {
		if (buffer.empty())
			return false;
		Message msg;
		vector<string> chunks = Utility::explode(buffer, DELIMITER);
		if (chunks.size() < 2) {
			LogNotice("Malformed message received");
			return false;
		}
		// Discard trash
		chunks.pop_back();

		try {
			msg.type = static_cast<Message::Types>(std::stoi(chunks.back()));
		}
		catch (...) {
			LogNotice("Invalid message type received");
			return false;
		}
		chunks.pop_back();
		msg.data = std::move(chunks);
		messages.push(msg);
	}
	return messages.size() > 0;
}

Message Messages::getMessage() {
	Message msg = std::move(messages.front());
	messages.pop();
	return msg;
}
