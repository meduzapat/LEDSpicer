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

using namespace LEDSpicer::Utilities;

Serial::Serial(const string& port) : port(port) {
#ifdef DRY_RUN
	// Ignore dry-run if /dev/null is used (unit test).
	if (port.empty() or port == "/dev/null") return;
	// Prepare a fake port to simulate serial communication.
	char slaveName[256] {0};
	if (openpty(&fakeFD, &fd, slaveName, nullptr, nullptr) != 0)
		throw Error("Can't open fake port ");
	this->port = string(slaveName);
	LogInfo("Port " + port + " replaced with fake port " + this->port);
	// Close it so the real open can take place.
	close(fd);
	fd = -1;
#endif
	}

Serial::~Serial() {
#ifdef DRY_RUN
	if (fakeFD >= 0) {
		close(fakeFD);
		fakeFD = -1;
	}
#endif
}

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

	if (fd >= 0) {
		LogDebug("Connection already open");
		return;
	}

	if ((fd = open(port.c_str(), O_RDWR | O_NOCTTY | O_SYNC)) < 0) {
		throw Error("Can't open port ") << port << ": " << strerror(errno);
	}

	try {
		// Set termios for non-blocking reads and standard 8N1.
		termios tty{};
		// Serial port config swiped from RXTX library (rxtx.qbang.org):
		if (tcgetattr(fd, &tty) != 0) {
			throw Error("Error getting serial port settings: ") << strerror(errno);
		}
		// Set raw mode
		cfmakeraw(&tty);

		// Explicit 8N1
		tty.c_cflag &= ~PARENB;        // No parity
		tty.c_cflag &= ~CSTOPB;        // 1 stop bit
		tty.c_cflag &= ~CRTSCTS;       // No HW flow control
		tty.c_cflag |= CREAD | CLOCAL; // Enable receiver, ignore modem ctrl lines

		// Non-blocking read
		tty.c_cc[VMIN]  = 0;
		tty.c_cc[VTIME] = 0;

		tty.c_oflag &= ~OPOST;  // Disable output processing
		tty.c_oflag &= ~ONLCR;  // Don't convert LF to CR-LF

		if (cfsetispeed(&tty, B115200) < 0 || cfsetospeed(&tty, B115200) < 0) {
			throw Error("Error setting baud rate: ") << strerror(errno);
		}

		if (tcsetattr(fd, TCSANOW, &tty) != 0) {
			throw Error("Error applying serial port settings: ") << strerror(errno);
		}

		// Set DTR high
		int status;
		if (ioctl(fd, TIOCMGET, &status) == -1) {
			LogWarning("Error getting modem status: " + string(strerror(errno)));
		}
		status |= TIOCM_DTR;
		if (ioctl(fd, TIOCMSET, &status) == -1) {
			LogWarning("Error setting DTR: " + string(strerror(errno)));
		}

		tcdrain(fd);

	}
	catch (Error& e) {
		if (fd >= 0) {
			close(fd);
			fd = -1;
		}
		throw;
	}
}

void Serial::disconnect() {
	LogDebug("Closing connection to " + port);

	if (fd < 0) {
		LogDebug("No active connection to close");
		return;
	}

	try {
		tcdrain(fd);

		// Try to clear DTR but don’t block closing if it fails
		int status;
		if (ioctl(fd, TIOCMGET, &status) != -1) {
			status &= ~TIOCM_DTR;
			ioctl(fd, TIOCMSET, &status);
		}
	}
	catch (...) {
		LogError("Error clearing DTR on " + port);
	}

	if (close(fd) < 0) {
		LogError("Error closing serial port " + port + ": " + strerror(errno));
	}
	fd = -1;
}

void Serial::transferToConnection(vector<uint8_t>& data) const {
	data.push_back('\0');
	Connection::transferToConnection(data);
	size_t totalWritten = 0;
	while (totalWritten < data.size()) {
		ssize_t written = write(fd, data.data() + totalWritten, data.size() - totalWritten);
		// Critical error (disconnection?).
		if (written < 0) throw Error("Fail to send: ") << strerror(errno);
		totalWritten += written;
	}
	//LogError("Data sent " + to_string(totalWritten));
	//tcdrain(fd); slow
	//fsync(fd); fast
}

vector<uint8_t> Serial::transferFromConnection(uint size) const {
	vector<uint8_t> response(size);
	size_t totalRead = 0;
	while (totalRead < size) {
		ssize_t bytesRead = read(fd, response.data() + totalRead, size - totalRead);
		if (bytesRead < 0) throw Error("Error reading from ") << port << ": " << strerror(errno);
		if (bytesRead == 0) break;
		totalRead += bytesRead;
	}
	// Clear input buffer
	tcflush(fd, TCIFLUSH);
	response.resize(totalRead);
	return response;
}

string Serial::findPortByUsbId(const string& id) {
	for (uint8_t c = 0; c < MAX_SERIAL_PORTS_TO_SCAN; ++c) {
		for (auto name : DEFAULT_SERIAL_PORTS) {
			string search(name + to_string(c));
			std::ifstream file(FS_PREPEND + search + FS_APPEND);
			if (not file.is_open()) continue;
			std::string line;
			while (std::getline(file, line)) {
				if (line.find(id) != std::string::npos) {
					LogDebug("Port found at " + string(FS_DIR) + search);
					return FS_DIR + search;
				}
			}
		}
	}
	return "";
}
