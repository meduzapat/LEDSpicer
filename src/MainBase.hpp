/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      MainBase.hpp
 * @since     Nov 18, 2018
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

#include "DataLoader.hpp"
#include "utilities/USB.hpp"

namespace LEDSpicer {

/**
 * LEDSpicer::MainBase
 * Class to keep all the main extra functionality and data.
 */
class MainBase {

public:

	MainBase();

	virtual ~MainBase();

	/**
	 * Starts the the LEDs test.
	 */
	void testLeds();

	/**
	 * Starts the Elements test.
	 */
	void testElements();

	/**
	 * Starts the dump configuration.
	 */
	void dumpConfiguration();

	/**
	 * Starts the dump profile.
	 */
	void dumpProfile();

	/**
	 * Waits for a defined amount of ms.
	 * @param milliseconds wasted keeps track of the wasted milliseconds.
	 */
	void wait(milliseconds wasted);

protected:

	/// Flag for the main loop.
	static bool running;

	/// Keeps messages incoming.
	Messages messages;

	/// Keeps a reference to profile in focus.
	Profile* currentProfile = nullptr;

	/**
	 * List of requested profiles that will be run in stack order.
	 * The last one is the one in focus (Pointed by currentProfile).
	 * If empty, the default profile is in focus.
	 */
	vector<Profile*> profiles;

	/// Starting point for the frame.
	high_resolution_clock::time_point
#ifdef BENCHMARK
		start,
		startTransfer;

	milliseconds
		timeAnimation,
		timeMessage,
		timeTransfer;
#else
		start;
#endif

	/**
	 * Functionality for test programs.
	 * @return
	 */
	Device* selectDevice();

	/**
	 * Send data to all devices.
	 */
	void sendData();
};

} // namespace
