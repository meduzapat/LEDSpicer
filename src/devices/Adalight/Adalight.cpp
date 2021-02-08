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

using namespace LEDSpicer::Devices;

bool Adalight::initialized = false;

void Adalight::resetLeds() {
	cout << "resetLeds" << endl;
	setLeds(0);
}

void Adalight::openDevice() {
	cout << "openDevice" << endl;
	if (initialized)
		throw Error(getFullName() + " device can only be loaded once");

	// Create serial port object and open serial port
	check(sp_get_port_by_name("/dev/ttyUSB0", &serialPort));
  check(sp_open(serialPort, SP_MODE_READ_WRITE));
	check(sp_set_baudrate(serialPort, 115200));
	check(sp_set_bits(serialPort, 8));
	check(sp_set_parity(serialPort, SP_PARITY_NONE));
	check(sp_set_stopbits(serialPort, 1));
	check(sp_set_flowcontrol(serialPort, SP_FLOWCONTROL_NONE));

	initialized = true;
}

void Adalight::closeDevice() {
	cout << "closeDevice" << endl;
	if (initialized){
		check(sp_close(serialPort));
	  sp_free_port(serialPort);
	}
}

string Adalight::getFullName() const {
	return "Adalight serial adapter";
}

void Adalight::drawHardwarePinMap() {
}


void Adalight::transfer() const {
	std::string serialData;
  int rgbLedNumber = getNumberOfLeds()/3; //adalight assumes rgb leds
	serialData.push_back('A');                              // Magic word
	serialData.push_back('d');
	serialData.push_back('a');
	serialData.push_back(((rgbLedNumber - 1) >> 8));   // LED count high byte
	serialData.push_back(((rgbLedNumber - 1) & 0xff)); // LED count low byte
	serialData.push_back((serialData[3] ^ serialData[4] ^ 0x55)); // Checksum


	for (uint8_t l = 0, t = LEDs.size(); l < t; ++l)
    serialData.push_back(LEDs[l]);

  int size = 6 + LEDs.size(); // 6 is header length


  /* We'll allow a 1 second timeout for send and receive. */
  unsigned int timeout = 1000;
  sp_blocking_write(serialPort, serialData.data(), size, timeout);

}

/* Helper function for error handling. */
int Adalight::check(enum sp_return result)
{
        char *error_message;

        switch (result) {
        case SP_ERR_ARG:
                throw Error("Adalight Error: Invalid argument.\n");
                abort();
        case SP_ERR_FAIL:
                error_message = sp_last_error_message();
                throw Error("Adalight Error: Failed: %s\n", error_message);
                sp_free_error_message(error_message);
                abort();
        case SP_ERR_SUPP:
                throw Error("Adalight Error: Not supported.\n");
                abort();
        case SP_ERR_MEM:
                throw Error("Adalight Error: Couldn't allocate memory.\n");
                abort();
        case SP_OK:
        default:
                return result;
        }
}
