/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Adalight.cpp
 * @since     Jan 10, 2021
 * @author    Bzzrd
 *
 * @copyright Copyright © 2021 Bzzrd
 * @copyright Copyright © 2022 Patricio A Rossi
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

void Adalight::detectPort() {
	for (const auto adaID : ADALIGHT_PRODUCT_IDS) {
		port = findPortByUsbId(adaID);
		if (not port.empty()) return;
	}
	throw Error("Unable to autodetect the serial port.");
}

void Adalight::transfer() const {

	// Ada Serial devices assumes RGB LEDs, cannot address individual LEDs.
	uint16_t
		numIndividualLeds(LEDs.size()),
		numLeds((numIndividualLeds / 3) - 1);
	/*
	ADAlight header.
	hi = (numLeds << 8) & 0xFF;
	lo = numLeds & 0xFF;
	checksum = hi ^ lo ^ 0x55
	*/
		vector<uint8_t> serialData {
			// Magic word
			'A', 'd', 'a',
			// LED count high byte
			static_cast<uint8_t>((numLeds >> 8) & 0xFF),
			// LED count low byte
			static_cast<uint8_t>(numLeds & 0xFF)
		};
	// Checksum
	serialData.push_back(static_cast<uint8_t>(serialData[3] ^ serialData[4] ^ 0x55));

	// Data
	serialData.insert(serialData.end(), LEDs.begin(), LEDs.begin() + numIndividualLeds);


	transferToConnection(serialData);
	/* for testing:*/
	/*auto data(transferFromConnection(512));
	string dataStr(data.begin(), data.end());
	std::replace(dataStr.begin(), dataStr.end(), '\n', ' ');
	dataStr.erase(std::remove(dataStr.begin(), dataStr.end(), '\r'), dataStr.end());
	LogInfo("R: " + dataStr);*/
	tcflush(fd, TCIFLUSH);

}

void Adalight::drawHardwareLedMap() {
	std::cout
		<< getFullName() << " LEDs " << LEDs.size() << " (" << LEDs.size() / 3 << " RGB LEDs)" << std::endl
		<< "Hardware LED map:" << std::endl << "0  2 G  +5v" << std::endl;
	for (uint16_t r = 0; r < LEDs.size(); r += 3) {
		std::cout <<
			" --- " << std::endl << static_cast<uint16_t>(r + 1) <<
			" " << static_cast<uint16_t>(r + 2) <<
			" " << static_cast<uint16_t>(r + 3) << std::endl;
	}
	std::cout << std::endl;
}
