/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      HowlerController.cpp
 * @since     Feb 5, 2020
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

#include "Howler.hpp"

using namespace LEDSpicer::Devices::WolfWareTech;

void Howler::resetLeds() {
	vector<uint8_t> data HOWLER_MSG(HOWLER_CMD_SET_GLOBAL_BRIGHTNESS, 0);
	transferToUSB(data);
	setLeds(0);
}

void Howler::drawHardwarePinMap() {
	for (uint8_t l = 0, t = LEDs.size(); l < t; ++l)
		setLed(l, l + 1);
	const char* const f = "                                          ";
	cout
		<< getFullName() << " Pins " << HOWLER_LEDS << endl
		<< "Hardware pin map:" << endl
		<< "G + X X X X  " << (int)*getLed(2) << "  " << (int)*getLed(1) << "  " << (int)*getLed(0) << " + +  "
		<< (int)*getLed(3) << "  " << (int)*getLed(4) << "  " << (int)*getLed(5) << " X X X X + G" << endl;
	for (uint8_t c = 0, c2 = 1, t = (LEDs.size() / 2) - 9; c < t ; c+=3, ++c2)
		cout
			<< "+" << (not c ? "              J 1          J 2            " : f) << " +" << endl
			<< (int)*getLed(c + 12) << f << (int)*getLed(t + c + 12) << endl
			<< (int)*getLed(c + 13) << " B" << static_cast<int>(c2)
			<< (c2 < 10 ? " " : "")
			<< "                                  B" << static_cast<int>(c2 + 13) << " "
			<< (int)*getLed(t + c + 13) << endl
			<< (int)*getLed(c + 14) << f << (int)*getLed(t + c + 14) << endl
			<< "X" << f << " X" << endl;

	cout
		<< "+" << f << " +" << endl
		<< (int)*getLed(90) << f << (int)*getLed(93) << endl
		<< (int)*getLed(91) << " HP 1                                HP 2 "
		<< (int)*getLed(94) << endl
		<< (int)*getLed(92) << f << (int)*getLed(95) << endl
		<< "G              J 3         J 4              G" << endl
		<< "G + X X X X  " << (int)*getLed(8) << "  " << (int)*getLed(7) << "  " << (int)*getLed(6) << " + + "
		<< (int)*getLed(9) << " " << (int)*getLed(10) << " " << (int)*getLed(11) << " X X X X + G" << endl << endl;
}

void Howler::transfer() {

	vector<uint8_t> data HOWLER_MSG(HOWLER_CMD_SET_GLOBAL_BRIGHTNESS, 0);
	transferToUSB(data);

	for (uint8_t c = 0, t = LEDs.size(); c < t ; ++c)
		if (LEDs[c]) {
			vector<uint8_t> data HOWLER_MSG(HOWLER_CMD_SET_INDIVIDUAL_LED, LEDs[c]);
			transferToUSB(data);
		}
}

uint16_t Howler::getProduct() {
	return HOWLER_PRODUCT + boardId - 1;
}

void Howler::transferToUSB(vector<uint8_t>& data) {
#ifdef DRY_RUN
	WolfWareTech::transferToUSB(data);
#else
	int transferred = 0;
	auto responseCode = libusb_interrupt_transfer(
		handle,
		HOWLER_IN_EP,
		data.data(),
		data.size(),
		&transferred,
		TIMEOUT
	);

	if (responseCode >= 0)
		return;

	LogDebug(
		"wValue: "  + Utility::hex2str(value) +
		" wIndex: "  + Utility::hex2str(interface) +
		" wLength: " + to_string(data.size())
	);
	throw Error(string(libusb_error_name(responseCode)) + " for " + getFullName());
#endif
}
