/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Serial.hpp
 * @since     Oct 8, 2023
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2023 Patricio A. Rossi (MeduZa)
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

// To handle terminal.
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring> // For strerror
#include "Connection.hpp"

#ifndef LSSERIAL_HPP_
#define LSSERIAL_HPP_ 1

#define DEFAULT_SERIAL_PORT "/dev/ttyUSB0"

namespace LEDSpicer {

/**
 * LEDSpicer::Serial
 * Class to connect to serial ports.
 */
class Serial : public Connection {

public:

	Serial() = delete;

	Serial(const string& port) : port(port) {}

	virtual ~Serial() = default;

	/**
	 * Returns the serial port.
	 * @return
	 */
	string getPort();

protected:

	termios tty;

	string port;

	int fd = 0;

	/**
	 * Attempts to detect the port.
	 */
	virtual void detectPort();

	/**
	 * Connects to the serial port.
	 */
	virtual void connect();

	/**
	 * Disonnects to the serial port.
	 */
	virtual void disconnect();

	/**
	 * Sends the payload.
	 */
	virtual void transferToConnection(vector<uint8_t>& data) const;

	/**
	 * Sends the payload.
	 */
	virtual vector<uint8_t> transferFromConnection(uint size) const;
};

} /* namespace */

#endif /* SERIAL_HPP_ */
