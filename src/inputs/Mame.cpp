/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Mame.cpp
 * @since     May 12, 2019
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

#include "Mame.hpp"

using namespace LEDSpicer::Inputs;

void Mame::drawConfig() {
	cout << SEPARATOR << endl << "Type: Mame" << endl;
	Input::drawConfig();
}

void Mame::process() {

	if (not active)
		return;

	if (not socks.isConnected())
		activate();

	string buffer;
	if (not socks.recive(buffer))
		return;

	buffer = Utility::extractChars(buffer, '*', 'z');

	if (buffer.empty())
		return;

	Log::debug("Message received " + buffer);

	if (buffer.find("mame_stop=1") != string::npos) {
		active = false;
		return;
	}

	if (Log::isLogging(LOG_DEBUG) and buffer.find("mame_start=1") != string::npos) {
		Log::debug("MAME running");
	}

	umap<string, bool> messages;

	auto parts = Utility::explode(buffer, '=');

	// Several outputs use digits, to make things simple zero ( 0 ) will be OFF, everything else will be ON

	if (itemsMap.count(parts[0]))
		messages[parts[0]] = parts[1][0] != '0';

	if (parts.size() > 2) {
		bool last = parts.back()[0] != '0';
		parts.pop_back();
		for (uint c = 1; c < parts.size(); c++) {
			if (itemsMap.count(parts[c].substr(1)))
				messages[parts[c].substr(1)] = parts[c + 1][0] != '0';
		}
		if (itemsMap.count(parts[parts.size() - 1].substr(1)))
			messages[parts[parts.size() - 1].substr(1)] = last;
	}

	for (auto& message : messages) {
		LogDebug("Sending: " + message.first + " " + (message.second ? "on" : "off"));
		if (message.second) {
			if (not controlledItems->count(message.first))
				controlledItems->emplace(message.first, itemsMap[message.first]);
		}
		else
			controlledItems->erase(message.first);
	}
}

void Mame::activate() {
	active = true;
	if (socks.isConnected()) {
		return;
	}
	try {
		// Open connection.
		socks.prepare(LOCALHOST, MAME_PORT, false, SOCK_STREAM);
	}
	catch (Error& e) {}
}

void Mame::deactivate() {
	socks.disconnect();
	active = false;
}
