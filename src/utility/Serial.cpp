/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Serial.cpp
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

#include "Serial.hpp"

using namespace LEDSpicer;

string Serial::getPort() {
	return port;
}

void Serial::detectPort() {
	throw Error("Hardware does not support serial port detection");
}

void Serial::connect() {
	if (port.empty()) {
		LogDebug("Port is not defined attempting detection...");
		detectPort();
	}
	LogDebug("Opening connection to " + port);
#ifdef DRY_RUN
	LogDebug("No Serial connection - DRY RUN");
	return;
#endif

	if (fd) {
		LogDebug("Connection already open");
		return;
	}

	if ((fd = open(port.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK)) < 0)
		throw Error("Can't open port " + port);

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

void Serial::disconnect() {
	LogDebug("Closing connection to " + port);
#ifdef DRY_RUN
	LogDebug("No disconnect - DRY RUN");
	return;
#endif
	close(fd);
}

void Serial::transferToConnection(vector<uint8_t>& data) const {
#ifdef SHOW_OUTPUT
	cout << "Data sent:" << endl;
	uint8_t count = MAX_DUMP_COLUMNS;
	for (auto b : data) {
		cout << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << static_cast<int>(b) << ' ';
		if (not --count) {
			count = MAX_DUMP_COLUMNS;
			cout << endl;
		}
	}
	cout << endl;
#endif

#ifndef DRY_RUN
	tcdrain(fd);
	int responseCode;
	if ((responseCode = write(fd, data.data(), data.size())) >= 0)
		return;
	LogError("Fail to send to serial: port " + port + " size " + std::to_string(data.size()));
	throw Error(string(strerror(responseCode)));
#endif
}

vector<uint8_t> Serial::transferFromConnection(uint size) const {
#ifdef DRY_RUN
	return std::vector<uint8_t>({'o','k','\n'});
#else
	std::vector<uint8_t> response(size);
	ssize_t bytesRead = read(fd, response.data(), response.size());
	if (bytesRead < 0) {
		LogError("Error reading from " + port);
	}
	else {
		response.resize(bytesRead);
	}
	return response;
#endif
}
