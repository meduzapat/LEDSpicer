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

void Howler::drawHardwarePinMap() {
	for (uint8_t l = 0, t = LEDs.size(); l < t; ++l)
		setLed(l, l + 1);
	const char* const f = "                                          ";
	cout
		<< getFullName() << " Pins " << HOWLER_LEDS << endl
		<< "Hardware pin map:" << endl
		<< "G + X X X X  " << static_cast<int>(*getLed(2)) << "  " << static_cast<int>(*getLed(1)) << "  " << static_cast<int>(*getLed(0)) << " + +  "
		<< static_cast<int>(*getLed(3)) << "  " << static_cast<int>(*getLed(4)) << "  " << static_cast<int>(*getLed(5)) << " X X X X + G" << endl;
	for (uint8_t c = 0, c2 = 1, t = (LEDs.size() / 2) - 9; c < t ; c+=3, ++c2)
		cout
			<< "+" << (not c ? "              J 1          J 2            " : f) << " +" << endl
			<< static_cast<int>(*getLed(c + 12)) << f << static_cast<int>(*getLed(t + c + 12)) << endl
			<< static_cast<int>(*getLed(c + 13)) << " B" << static_cast<int>(c2)
			<< (c2 < 10 ? " " : "")
			<< "                                  B" << static_cast<int>(c2 + 13) << " "
			<< static_cast<int>(*getLed(t + c + 13)) << endl
			<< static_cast<int>(*getLed(c + 14)) << f << static_cast<int>(*getLed(t + c + 14)) << endl
			<< "X" << f << " X" << endl;

	cout
		<< "+" << f << " +" << endl
		<< static_cast<int>(*getLed(90)) << f << static_cast<int>(*getLed(93)) << endl
		<< static_cast<int>(*getLed(91)) << " HP 1                                HP 2 "
		<< static_cast<int>(*getLed(94)) << endl
		<< static_cast<int>(*getLed(92)) << f << static_cast<int>(*getLed(95)) << endl
		<< "G              J 3         J 4              G" << endl
		<< "G + X X X X  " << static_cast<int>(*getLed(8)) << "  " << static_cast<int>(*getLed(7)) << "  " << static_cast<int>(*getLed(6)) << " + + "
		<< static_cast<int>(*getLed(9)) << " " << static_cast<int>(*getLed(10)) << " " << static_cast<int>(*getLed(11)) << " X X X X + G" << endl << endl;
}

void Howler::transfer() const {
	vector<uint8_t> data(24);
	data = howlerBankA(1, 0);
	transferToConnection(data);
	data = howlerBankB(2, 0);
	transferToConnection(data);
	data = howlerBankA(3, 1);
	transferToConnection(data);
	data = howlerBankB(4, 1);
	transferToConnection(data);
	data = howlerBankA(5, 2);
	transferToConnection(data);
	data = howlerBankB(6, 2);
	transferToConnection(data);
}

uint16_t Howler::getProduct() const {
	return HOWLER_PRODUCT + boardId - 1;
}

int Howler::send(vector<uint8_t>& data) const {

	int transferred = 0;
	return libusb_interrupt_transfer(
		handle,
		HOWLER_IN_EP,
		data.data(),
		data.size(),
		&transferred,
		USB_TIMEOUT
	);
}
