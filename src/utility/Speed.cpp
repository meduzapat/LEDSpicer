/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Speed.cpp
 * @since     May 27, 2019
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

#include "Speed.hpp"

using namespace LEDSpicer;

Speed::Speed(string speed) {
	if (not speed.empty())
		this->speed = str2speed(speed);
}

string Speed::speed2str(Speeds speed) {
	switch (speed) {
	case Speeds::VeryFast:
		return "Very Fast";
	case Speeds::Fast:
		return "Fast";
	case Speeds::Normal:
		return "Normal";
	case Speeds::Slow:
		return "Slow";
	case Speeds::VerySlow:
		return "Very Slow";
	}
	return "";
}

Speed::Speeds Speed::str2speed(const string& speed) {
	if (speed == "VeryFast")
		return Speeds::VeryFast;
	if (speed == "Fast")
		return Speeds::Fast;
	if (speed == "Normal")
		return Speeds::Normal;
	if (speed == "Slow")
		return Speeds::Slow;
	if (speed == "VerySlow")
		return Speeds::VerySlow;
	throw Error("Invalid speed " + speed);
}

void Speed::drawConfig() {
	cout << "Speed: " << speed2str(speed) << endl;
}

