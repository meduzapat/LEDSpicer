/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Restrictor.cpp
 * @since     Jul 7, 2020
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2020 Patricio A. Rossi (MeduZa)
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

#include "Restrictor.hpp"

using namespace LEDSpicer::Restrictors;

void Restrictor::initialize() {
	LogDebug("Initializing Restrictor " + getName());
	connect();
	claimInterface();
}

void Restrictor::terminate() {
	LogDebug("Disconnect " + getName());
	disconnect();
}

uint8_t Restrictor::getMaxIds() const {
	return 1;
}

void Restrictor::rotate(Ways ways) {
	umap<string, Ways> playersData;
	for (auto& p : players)
		playersData.emplace(p.first, ways);
	rotate(playersData);
}

Restrictor::Ways Restrictor::str2ways(const string& ways) {
	if (ways == "1" or ways == "2" or ways == "strange2")
		return Ways::w2;
	if (ways == "vertical2")
		return Ways::w2v;
	if (ways == "3 (half4)" or ways == "4")
		return Ways::w4;
	if (ways == "4x")
		return Ways::w4x;
	if (ways == "49")
		return Ways::w49;
	if (ways == "16")
		return Ways::w16;
	if (ways == "analog")
		return Ways::analog;
	if (ways == "mouse")
		return Ways::mouse;
	if (ways == "rotary8")
		return Ways::rotary8;
	if (ways == "rotary12")
		return Ways::rotary12;
	return Ways::w8;
}

string Restrictor::ways2str(Ways ways) {
	switch (ways) {
	case Ways::w2:
		return "horizontal 2 ways";
	case Ways::w2v:
		return "vertical 2 ways";
	case Ways::w4:
		return "4 ways";
	case Ways::w4x:
		return "diagonal 4 ways";
	case Ways::w8:
		return "8 ways";
	case Ways::w16:
		return "16 ways";
	case Ways::w49:
		return "49 ways";
	case Ways::analog:
		return "analog";
	case Ways::mouse:
		return "mouse";
	case Ways::rotary8:
		return "rotary 8";
	case Ways::rotary12:
		return "rotary 12";
	}
	return "";
}

bool Restrictor::isRotary(const Ways& ways) {
	return (ways == Ways::rotary8 or ways == Ways::rotary12);
}

Restrictor::Ways Restrictor::getWay(const umap<string, Ways>& playersData, bool rotary) const {
	Ways way = Ways::invalid;
	for (auto& p : players)
		if (playersData.count(p.first) and rotary == isRotary(playersData.at(p.first))) {
			way = playersData.at(p.first);
			break;
		}
	return way;
}

void Restrictor::disconnect() {

#ifdef DRY_RUN
	LogDebug("No disconnect - DRY RUN");
#endif

	if (not handle)
		return;

	libusb_release_interface(handle, interface);

#ifdef DEVELOP
	LogDebug("Closing interface: " + to_string(interface));
#endif
	libusb_close(handle);
	handle = nullptr;
}

