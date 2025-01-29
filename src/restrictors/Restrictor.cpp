/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Restrictor.cpp
 * @since     Jul 7, 2020
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

#include "Restrictor.hpp"

using namespace LEDSpicer::Restrictors;

void Restrictor::initialize() {

	if (dumpMode)
		return;

	LogDebug("Initializing Restrictor " + getFullName());
	openHardware();
}

void Restrictor::terminate() {

	if (dumpMode)
		return;

	LogDebug("Disconnect " + getFullName());
	closeHardware();
}

uint8_t Restrictor::getMaxIds() const {
	return RESTRICTOR_SINGLE_ID;
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

const Restrictor::Ways Restrictor::strWays2Ways(const std::string& strWays) {
	try {
		int intValue(std::stoi(strWays));
		if (intValue >= static_cast<int>(Ways::invalid) && intValue <= static_cast<int>(Ways::rotary12)) {
			return static_cast<Ways>(intValue);
		}
		throw;
	}
	catch (...) {
		return Ways::invalid;
	}
}

const string Restrictor::ways2StrWays(Ways ways) {
	if (ways == Ways::invalid)
		return "invalid";
	return to_string(static_cast<int>(ways));
}

bool Restrictor::isRotary(const Ways& ways) {
	return (ways == Ways::rotary8 or ways == Ways::rotary12);
}

const string Restrictor::getProfileStr(const string& profile) {
	auto parts(Utility::explode(profile, '_'));
	return "Player " + parts[0] + ", Joystick " + parts[1];
}
