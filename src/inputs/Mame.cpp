/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Mame.cpp
 * @since     May 12, 2019
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2018 - 2026 Patricio A. Rossi (MeduZa)
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

void Mame::drawConfig() const {
	cout << SEPARATOR << endl << "Type: Mame" << endl;
	Input::drawConfig();
}

void Mame::process() {

	if (not active) return;

	if (not socks.isConnected()) activate();

	string buffer;
	if (not socks.receive(buffer)) return;

	// Mame sent carriage return
	std::replace(buffer.begin(), buffer.end(), '\r', '\n');
	buffer.erase(std::remove(buffer.begin(), buffer.end(), ' '), buffer.end());

	if (buffer.empty()) return;

	// Process each line separately.
	for (string& line : Utility::explode(buffer, '\n')) {

		if (line.empty()) continue;

		LogDebug("Message received " + line);

		if (line.find("mame_stop") != string::npos) {
			active = false;
			return;
		}

		if (line.find("mame_start") != string::npos) continue;

		auto parts = Utility::explode(line, '=');
		if (parts.size() != 2) continue;

		if (not itemsUMap.exists(parts[0])) continue;

		bool on = not parts[1].empty() and parts[1][0] != '0';
		LogDebug("Sending: " + parts[0] + " " + (on ? "on" : "off"));

		if (on) {
			if (not controlledItems.exists(parts[0]))
				controlledItems.emplace(parts[0], itemsUMap[parts[0]]);
		}
		else {
			controlledItems.erase(parts[0]);
		}
	}
}

void Mame::activate() {
	active = true;
	if (socks.isConnected()) return;
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
