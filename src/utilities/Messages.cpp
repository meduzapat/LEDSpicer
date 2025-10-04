/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Messages.cpp
 *
 * @since     Jul 8, 2018
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

#include "Messages.hpp"

using namespace LEDSpicer::Utilities;

bool Messages::read() {

	string buffer;
	if (receive(buffer)) {

		if (buffer.empty()) return false;

		Message msg;
		vector<string> chunks(Utility::explode(buffer, RECORD_SEPARATOR));

		if (chunks.size() == 1) {
			LogNotice("Malformed message received");
			return false;
		}

		try {
			// Discard tale if empty.
			if (chunks.back().empty()) chunks.pop_back();
			msg.setType(static_cast<Message::Types>(std::stoi(chunks.back())));
			if (msg.getType() == Message::Types::Invalid)
				throw 1;
			chunks.pop_back();

			msg.setFlags(std::stoi(chunks.back()));
		}
		catch (...) {
			LogNotice("Invalid message type received");
			return false;
		}
		chunks.pop_back();
		msg.setData(std::move(chunks));
		messages.push(msg);
	}
	return messages.size() > 0;
}

Message Messages::getMessage() {
	Message msg = std::move(messages.front());
	messages.pop();
	return msg;
}
