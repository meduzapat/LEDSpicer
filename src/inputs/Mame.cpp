/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Mame.cpp
 * @since     May 12, 2019
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

#include "Mame.hpp"

using namespace LEDSpicer::Inputs;

void Mame::process() {

	if (not active)
		return;

	if (not socks.isConnected())
		activate();

	string buffer;
	if (not socks.recive(buffer))
		return;

	// Message ends at line feed, discard the tail.
	size_t s = buffer.find(13);
	if (s != string::npos)
		buffer = buffer.substr(0, s);

	if (buffer.empty())
		return;

	if (buffer == "mame_stop = 1") {
		active = false;
		return;
	}

	auto parts = Utility::explode(buffer, '=');
	if (parts.size() != 2)
		return;

	Utility::trim(parts[0]);
	if (Log::isLogging(LOG_DEBUG) and parts[0] == "mame_start") {
		Utility::trim(parts[1]);
		Log::debug("MAME running " + parts[1]);
	}

	uint8_t n = std::atoi(parts[1].c_str());
	if (elementMap.count(parts[0])) {
		LogDebug("e: " + parts[0] + " " + (n ? "on" : "off"));
		auto& e = elementMap[parts[0]];
		if (n)
			controlledElements->emplace(e.element->getName() + parts[0], &e);
		else
			controlledElements->erase(e.element->getName() + parts[0]);
	}

	if (groupMap.count(parts[0])) {
		LogDebug("g: " + parts[0] + " " + (n ? "on" : "off"));
		auto& g = groupMap[parts[0]];
		if (n)
			controlledGroups->emplace(g.group->getName() + parts[0], &g);
		else
			controlledGroups->erase(g.group->getName() + parts[0]);
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
