/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Hardware.hpp
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

// For ints.
#include <cstdint>

#include "utility/Log.hpp"
#include "utility/Utility.hpp"

#ifndef HARDWARE_HPP_
#define HARDWARE_HPP_ 1

namespace LEDSpicer {

/**
 * LEDSpicer::Hardware
 * Generic functionality for connectable hardware.
 */
class Hardware {

public:

	Hardware() = delete;

	Hardware(const string& name) : name(name) {}

	virtual ~Hardware() = default;

	/**
	 * Initialize the hardware
	 */
	virtual void initialize() = 0;

	/**
	 * Terminate the hardware.
	 */
	virtual void terminate() = 0;

	/**
	 * Returns the hardware full name.
	 * @return
	 */
	virtual string getFullName() const = 0;

	/**
	 * Calling this method will make the Serial not to connect to the real hard.
	 */
	void static setDumpMode();

protected:

	/// If set to true will not connect to the real hardware.
	static bool dumpMode;

	/// The hardware name.
	string name;

	/**
	 * This method will be called to start a hardware.
	 */
	virtual void openHardware() = 0;

	/**
	 * This method will be called to close a hardware.
	 */
	virtual void closeHardware() = 0;

};

} /* namespace LEDSpicer */

#endif /* HARDWARE_HPP_ */
