/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Serial.cpp
 * @since     Oct 8, 2023
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
#ifdef DRY_RUN
		LogDebug("No port detection - DRY RUN");
#else
		detectPort();
#endif
	}
	LogDebug("Opening connection to " + port);
#ifdef DRY_RUN
	LogDebug("No Serial connection - DRY RUN");
	return;
#endif

	if (fd > 0) {
		LogDebug("Connection already open");
		return;
	}

	if ((fd = open(port.c_str(), O_RDWR | O_NOCTTY | O_SYNC)) < 0) {
		throw Error("Can't open port " + port);
	}

	try {
		termios tty;
		// Serial port config swiped from RXTX library (rxtx.qbang.org):
		if (tcgetattr(fd, &tty) != 0) {
			throw Error("Error getting serial port settings");
		}
		tty.c_iflag     = INPCK;
		tty.c_lflag     = 0;
		tty.c_oflag     = 0;
		tty.c_cflag     = CREAD | CS8 | CLOCAL;
		tty.c_cc[VMIN]  = 0;
		tty.c_cc[VTIME] = 0;
		cfsetispeed(&tty, B115200);
		cfsetospeed(&tty, B115200);
		if (tcsetattr(fd, TCSANOW, &tty) != 0) {
			throw Error("Error setting serial port attributes");
		}
		// Set DTR
		int status;
		if (ioctl(fd, TIOCMGET, &status) == -1) {
			throw Error("Error getting modem status");
		}
		status |= TIOCM_DTR;
		if (ioctl(fd, TIOCMSET, &status) == -1) {
			throw Error("Error setting DTR");
		}
		tcdrain(fd);
	}
	catch (Error& e) {
		close(fd);
		throw e;
	}
}

void Serial::disconnect() {
	LogDebug("Closing connection to " + port);
#ifdef DRY_RUN
	LogDebug("No disconnect - DRY RUN");
	return;
#endif
	tcdrain(fd);
	// Clear DTR
	int status;
	ioctl(fd, TIOCMGET, &status);
	status &= ~TIOCM_DTR;
	ioctl(fd, TIOCMSET, &status);
	struct termios tty;
	tcgetattr(fd, &tty);
	// Close the serial port
	if (close(fd) < 0) {
		fd = 0;
		throw Error("Error closing serial port");
	}
	fd = 0;
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
	// Most devices right now expect OK as a good answer.
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

string Serial::findPortByUsbId(const string& id) {
	for (uint8_t c = 0; c < MAX_SERIAL_PORTS_TO_SCAN; ++c) {
		for (const string& name : DEFAULT_SERIAL_PORTS) {
			string search(name + to_string(c));
			std::ifstream file(FS_PREPEND + search + FS_APPEND);
			if (not file.is_open())
				continue;
			std::string line;
			while (std::getline(file, line)) {
				if (line.find(id) != std::string::npos) {
					LogDebug("Port found at " FS_DIR + search);
					return FS_DIR + search;
				}
			}
		}
	}
	return "";
}
