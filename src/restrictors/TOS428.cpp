/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      TOS428.cpp
 * @since     Oct 8, 2023
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2024 Patricio A. Rossi (MeduZa)
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

#include "TOS428.hpp"

using namespace LEDSpicer::Restrictors;

uint8_t TOS428::getMaxIds() const {
	return TOS428_MAX_ID;
}

void TOS428::rotate(const umap<string, Ways>& playersData) {

	Ways way = playersData.begin()->second;

	// This restrictor support sending all
	bool all = true;
	if (playersData.size() > 1)
		for (const auto& p : playersData) {
			if (way != p.second) {
				all = false;
				break;
			}
		}
	else
		all = false;

	if (all) {
		createPackage(way, 0);
		return;
	}
	for (const auto& p : playersData) {
		createPackage(p.second, players.at(p.first));
	}
}

void TOS428::detectPort() {
	for (uint8_t c = 0; c < TOS428_MAX_PORT; ++c) {
		(port = string(TOS428_PORT)).push_back(c + '0');
		RestrictorSerial::openHardware();
		if (sendGreetings())
			break;
		RestrictorSerial::closeHardware();
	}
}

void TOS428::createPackage(Ways way, uint8_t target) {
	uint8_t w;
	switch (way) {
	case Ways::w2:
	case Ways::w2v:
	case Ways::w4:
		w = '4';
	break;
	default:
		w = '8';
	}
	LogDebug("Rotating " + getFullName() + (target == 0 ? " all restrictors" : " restrictor " + std::to_string(target)) + " to " + ways2str(way));
	vector<uint8_t> data TOS428_DATA(static_cast<uint8_t>(target + '0'), w);
	transferToConnection(data);
	std::this_thread::sleep_for(std::chrono::microseconds(1000));
}

bool TOS428::sendGreetings() {
	LogDebug("Sending greetings");
	vector<uint8_t> data {'g', 'e', 't', 'w', 'e', 'l', 'c', 'o', 'm', 'e'};
	transferToConnection(data);
	// Have to add some delay or the message gets cut.
	std::this_thread::sleep_for(std::chrono::microseconds(2000));
	data = transferFromConnection(128);
	string dataStr(data.begin(), data.end());
	std::this_thread::sleep_for(std::chrono::microseconds(1000));
	LogDebug("Answer: " + dataStr);
	return (dataStr.find(TOS428_FLAG) != string::npos);
}
