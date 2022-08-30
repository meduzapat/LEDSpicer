/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Adalight.cpp
 * @since     Jan 10, 2021
 * @author    Bzzrd
 *
 * @copyright Copyright © 2021 Bzzrd
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

#include "Adalight.hpp"

using namespace LEDSpicer::Devices::Adalight;

void Adalight::resetLeds() {
	cout << "resetLeds" << endl;
	setLeds(0);
}

void Adalight::openDevice() {

	DeviceSerial::openDevice();
	termios tty;
	// Serial port config swiped from RXTX library (rxtx.qbang.org):
	tcgetattr(fd, &tty);
	tty.c_iflag     = INPCK;
	tty.c_lflag     = 0;
	tty.c_oflag     = 0;
	tty.c_cflag     = CREAD | CS8 | CLOCAL;
	tty.c_cc[VMIN]  = 0;
	tty.c_cc[VTIME] = 0;
	cfsetispeed(&tty, B115200);
	cfsetospeed(&tty, B115200);
	tcsetattr(fd, TCSANOW, &tty);
}

void Adalight::transfer() const {
	// Ada Serial devices assumes RGB LEDs
	uint8_t t = LEDs.size() / 3;
	vector<uint8_t> serialData {
		// Magic word
		'A', 'd', 'a',
		// LED count high byte
		0,
		// LED count low byte
		static_cast<uint8_t>(t - 1),
	};
	// Checksum
	serialData.push_back(static_cast<uint8_t>(serialData[4] ^ 0x55));

	for (uint16_t l = 0, t = LEDs.size(); l < t; ++l)
		serialData.push_back(LEDs[l]);

	tcdrain(fd);
	if (write(fd, serialData.data(), serialData.size()) < 0)
		LogError("Fail to send the serial data");
}

string Adalight::getFullName() const {
	return "Adalight serial adapter";
}

void Adalight::drawHardwarePinMap() {
	cout
		<< getFullName() << " Pins " << LEDs.size() << endl
		<< "Hardware pin map:" << endl << "0  2 G  +5v" << endl;
	for (uint8_t r = 0; r < LEDs.size(); r += 3) {
		LEDs[r] = r + 1;
		LEDs[r + 1] = r + 2;
		LEDs[r + 2] = r + 3;
		cout <<
			" --- " << endl <<
			(int)*getLed(r) << " " << (int)*getLed(r + 1) << " " << (int)*getLed(r + 2) << endl;
	}
	cout << endl;
}
