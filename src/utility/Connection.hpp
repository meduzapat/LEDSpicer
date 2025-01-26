/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Connection.hpp
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

#include "utility/Log.hpp"
#include "utility/Utility.hpp"

#ifdef SHOW_OUTPUT
#include <iomanip>
#endif

#ifndef LSCONNECTION_HPP_
#define LSCONNECTION_HPP_ 1

/// The number of values to display when dumping LEDs (multiply x 2 to get the actual size).
#define MAX_DUMP_COLUMNS 40

namespace LEDSpicer {

/**
 * LEDSpicer::Connection
 * Interface to define generic connection methods.
 */
class Connection {

public:

	Connection() = default;

	virtual ~Connection() = default;

protected:

	/**
	 * Connects to the serial port.
	 */
	virtual void connect() = 0;

	/**
	 * Disconnects from the serial port.
	 */
	virtual void disconnect() = 0;

	/**
	 * Prepares the data and send it to the connection function.
	 * @param data
	 */
	virtual void transferToConnection(vector<uint8_t>& data) const = 0;

	/**
	 * Reads from connection.
	 * @param size the amount of data to read.
	 * @return
	 */
	virtual vector<uint8_t> transferFromConnection(uint size) const = 0;

};

} /* namespace LEDSpicer */

#endif /* LSCONNECTION_HPP_ */
