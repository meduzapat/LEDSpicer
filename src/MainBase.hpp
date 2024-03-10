/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      MainBase.hpp
 * @since     Nov 18, 2018
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2018 - 2024 Patricio A. Rossi (MeduZa)
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

// To handle c signals.
#include <csignal>
using std::signal;

#include "DataLoader.hpp"
#include "devices/DeviceUSB.hpp"

#ifndef MAINBASE_HPP_
#define MAINBASE_HPP_ 1

namespace LEDSpicer {

using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;


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
	static void wait(milliseconds wasted);

protected:

	/// Flag for the main loop.
	static bool running;

	/// Keeps messages incoming.
	Messages messages;

	static Profile* currentProfile;

	/**
	 * Stack of profiles.
	 */
	static vector<Profile*> profiles;

	/// Keeps a list of always on elements for the current profile.
	static umap<string, Element::Item> alwaysOnElements;

	/// Keeps a list of always on groups for the current profile.
	static umap<string, Group::Item> alwaysOnGroups;

	/// Starting point for the frame.
	static high_resolution_clock::time_point start;

	/**
	 * Functionality for test programs.
	 * @return
	 */
	Device* selectDevice();

	/**
	 * Attempts to load profiles from a list of names.
	 * @param data
	 * @return nullptr if all failed.
	 */
	Profile* tryProfiles(const vector<string>& data);

	/**
	 * Attempts create a profile with the values provided.
	 * @param name
	 * @return null if failed.
	 */
	Profile* craftProfile(const string& name, const string& elements, const string& groups);

	/**
	 * Terminates the current profile and reset everything.
	 */
	void terminateCurrentProfile();

	/**
	 * Send data to all devices.
	 */
	void sendData();

	/**
	 * Clears always on Elements and Groups and the Input states.
	 */
	void clearOverrides();
};

} /* namespace LEDSpicer */

#endif /* MAINBASE_HPP_ */
