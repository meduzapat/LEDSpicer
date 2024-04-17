/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Network.cpp
 * @since     Jul 24, 2020
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

#include "Network.hpp"

using namespace LEDSpicer::Inputs;

LEDSpicer::Socks Network::sock;

void Network::activate() {
	if (sock.isConnected())
		return;
	sock.prepare(LOCALHOST, DEFAULT_PORT, true);
}

void Network::deactivate() {
	sock.disconnect();
}

void Network::process() {

	if (not sock.isConnected())
		activate();

	string buffer;
	if (not sock.recive(buffer))
		return;

	Log::debug("Message received " + buffer);
	if (buffer.empty())
		return;
	buffer = Utility::extractChars(buffer, '!', RECORD_SEPARATOR);
	for (string& entry : Utility::explode(buffer, RECORD_SEPARATOR)) {
#ifdef DEVELOP
		LogDebug("Processing " + entry);
#endif
		if (itemsMap.count(entry)) {
			if (controlledItems.count(entry)) {
				LogDebug("map " + entry +" Off");
				controlledItems.erase(entry);
			}
			else {
				LogDebug("map " + entry +" On");
				controlledItems.emplace(entry, itemsMap[entry]);
			}
		}
	}
}
