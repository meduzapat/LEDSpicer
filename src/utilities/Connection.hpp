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

#include "Log.hpp"
#include "Utility.hpp"

#pragma once

/// The number of values to display when dumping LEDs (multiply x 2 to get the actual size).
#define MAX_DUMP_COLUMNS 40

namespace LEDSpicer::Utilities {

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
	 * Connects to the destination.
	 */
	virtual void connect() = 0;

	/**
	 * Disconnects from the destination.
	 */
	virtual void disconnect() = 0;

	/**
	 * Prepares the data and send to the connection.
	 * @param data
	 */
	virtual void transferToConnection(vector<uint8_t>& data) const {
#ifdef SHOW_OUTPUT
		std::stringstream ss;
		ss << "Data to be sent:" << std::endl;
		uint8_t count = MAX_DUMP_COLUMNS;
		for (auto b : data) {
			ss << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << static_cast<int>(b) << ' ';
			if (not --count) {
				count = MAX_DUMP_COLUMNS;
				ss << std::endl;
			}
		}
		ss << std::endl;
		LogDebug(ss.str());
#else
		// To avoid unused parameter warning.
		static_cast<void>(data);
#endif
	}

	/**
	 * Reads from connection.
	 * @param size the amount of data to read.
	 * @return
	 */
	virtual vector<uint8_t> transferFromConnection(uint size) const = 0;

};

} // namespace
