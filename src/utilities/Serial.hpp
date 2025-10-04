/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Serial.hpp
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

#ifdef DRY_RUN
// To handle fake terminal.
#include <pty.h>
#else
// To handle terminal.
#include <termios.h>
#endif
// For Open.
#include <fcntl.h>
// For ioctl.
#include <sys/ioctl.h>

#include "Connection.hpp"

#pragma once

constexpr char FS_PREPEND[] = "/sys/class/tty/";
constexpr char FS_APPEND[]  = "/device/uevent";
constexpr char FS_DIR[]     = "/dev/";

namespace LEDSpicer::Utilities {

/**
 * LEDSpicer::Serial
 * Class to connect to serial ports.
 */
class Serial : public Connection {

public:

	Serial() = delete;

	/**
	 * Creates a new Serial connection handler.
	 * @param port The serial port path (e.g., "/dev/ttyUSB0"). If empty, it will attempt to auto-detect.
	 */
	Serial(const string& port);

	virtual ~Serial();

	/**
	 * Returns the serial port.
	 * @return
	 */
	string getPort();

protected:

	/// The serial port path.
	string port;

	/// File descriptor for the serial port.
	int fd = -1;

#ifdef DRY_RUN
	/// fake fds to keep the pty open.
	int fakeFD = -1;
#endif

	/**
	 * Attempts to detect the port.
	 */
	virtual void detectPort();

	/**
	 * Connects to the serial port.
	 */
	void connect() override;

	/**
	 * Disconnects to the serial port.
	 */
	void disconnect() override;

	/**
	 * Sends a payload.
	 * @see Connection::transferToConnection
	 */
	void transferToConnection(vector<uint8_t>& data) const override;

	/**
	 * Receives a payload.
	 * @see Connection::transferFromConnection
	 */
	vector<uint8_t> transferFromConnection(uint size) const override;

	/**
	 * Finds a serial port by USB vendor/product ID (e.g., "0403:6001").
	 * @param id The USB ID string to search for.
	 * @return The port path or empty if not found.
	 */
	static string findPortByUsbId(const string& id);
};

} // namespace
