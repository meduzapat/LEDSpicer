/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file		Socks.hpp
 * @author		Patricio A. Rossi (MeduZa)
 * 
 * @copyright	Copyright © 2015 - 2018 Patricio A. Rossi (MeduZa)
 *
 * @copyright	ledspicer is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * @copyright	ledspicer is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * @copyright	You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

// To use strings
#include <string>
using std::string;

// For gethostbyname()
#include <netdb.h>

#include "Error.hpp"
#include "Log.hpp"

#ifndef SRC_UTILITY_SOCKS_HPP_
#define SRC_UTILITY_SOCKS_HPP_ 1

namespace LEDSpicer {

/**
 * Socks
 *
 * Class to handle IP connections using sockets.
 */
class Socks {

public:

	/**
	 * Creates a new socket instance, but does not connect.
	 */
	Socks() {}

	/**
	 * Creates a new socked connection to hostAddress on the port hostPort.
	 *
	 * @param[in] hostAddress IP address.
	 * @param[in] hostPort port number.
	 * @param[in] bind, if set will open a listening port, server mode, false will create a client mode.
	 * @throws Error if fails to connect.
	 */
	Socks(const string& hostAddress, const string& hostPort, bool bind = false);

	/**
	 *
	 * @param hostAddress
	 * @param hostPort
	 * @param bind true will bind a port, false (default) will connect to a port.
	 */
	void prepare(const string& hostAddress, const string& hostPort, bool bind);

	/**
	 * Checks if is connected.
	 *
	 * @return boolean true if connected.
	 */
	bool isConnected() {
		return sockFB != 0;
	}

	/**
	 * Sends message.
	 *
	 * @param[in] message a string to send to the sock.
	 * @return true on success, false on fail, if the message is empty will return true but nothing will be done.
	 */
	bool send(const string& message) throw();

	/**
	 * Retrieves message.
	 *
	 * @param[out] buffer
	 * @return boolean true on success, false on fail, if nothing is received true is return.
	 */
	bool recive(string& buffer);

	/**
	 * Closes the connection.
	 */
	void disconnect();

	/**
	 * Clean up.
	 */
	~Socks();

private:

	/// Socket frame buffer.
	int sockFB = 0;

};

}

#endif // SRC_UTILITY_SOCKS_HPP_
