/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Main.hpp
 * @since     Jun 6, 2018
 *
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2018 - 2026 Patricio A. Rossi (MeduZa)
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
 *
 * @mainpage LEDSpicer LEDs controller.
 * <p>LEDSpicer is a LEDs controllers daemon that will handle the output of different programs based on their configurations.<br>
 * Most of that programs will be emulators and games, but can be used with other applications in mind.</p>
 */

#include "MainBase.hpp"

namespace LEDSpicer {

using Devices::Device;
using namespace Transitions;

/**
 * LEDSpicer::main
 */
class Main : public MainBase {

public:

	using MainBase::MainBase;

	/**
	 * Starts the main loop.
	 */
	void run();

	/**
	 * Stops the main loop.
	 */
	static void terminate();

protected:

	/**
	 * Attempts to load profiles from a list of names.
	 * @param data a list of profiles to try.
	 * @return the new loaded profile, nullptr if all failed.
	 */
	Profile* tryProfiles(const vector<string>& data);

	/**
	 * Attempts create a profile with the values provided.
	 * Will check cache for an existing profile first.
	 * @param name
	 * @return the new crafted profile or null if failed.
	 */
	Profile* craftProfile(const string& name, const string& elements, const string& groups);

	/**
	 * Initiates the process or replacing the current profile.
	 *
	 * @param to
	 * @param store if true will store the profile in the profiles stack.
	 */
	void changeProfile(Profile* to, bool store);

};

/**
 * Main function.
 * Handles command line and executes the program.
 *
 * @param argc
 * @param argv
 * @return exit code.
 */
int main(int argc, char **argv);

} // namespace
