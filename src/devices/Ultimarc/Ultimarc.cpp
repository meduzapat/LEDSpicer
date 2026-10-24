/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Ultimarc.cpp
 * @since     Jul 27, 2026
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

#include "Ultimarc.hpp"

using namespace LEDSpicer::Devices::Ultimarc;

void Ultimarc::transferPairs(vector<uint8_t> message, uint8_t command) const {

	// Announce the stream.
	message[command]     = 0xFE;
	message[command + 1] = 0;
	transferToConnection(message);

	for (uint16_t c = 0; c < LEDs.size(); c += 2) {
		message[command]     = LEDs[c];
		message[command + 1] = LEDs[c + 1];
		transferToConnection(message);
	}
}
