/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Main.cpp
 * @since     Jun 6, 2018
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2018 - 2020 Patricio A. Rossi (MeduZa)
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

#include "Main.hpp"
#include <fcntl.h>

#ifdef DEVELOP
#include <execinfo.h>
#endif

using namespace LEDSpicer;

high_resolution_clock::time_point Main::start;

void signalHandler(int sig) {

	switch (sig) {
	case SIGTERM:
	case SIGABRT:
	case SIGQUIT:
	case SIGINT:
		LogNotice(PACKAGE_NAME " terminated by signal");
		Main::terminate();
		return;
	case SIGHUP:
		LogNotice(PACKAGE_NAME " received re load configuration signal");
		// TODO: reload the profiles and restart everything?
		return;
	case SIGSEGV:
	case SIGILL:
		// Display back trace.
#ifdef DEVELOP
		void* array[10];
		size_t size = backtrace(array, 10);
		backtrace_symbols_fd(array, size, STDERR_FILENO);
#endif
		exit(EXIT_FAILURE);
		break;
	}
}

void Main::run() {

	LogInfo(PACKAGE_NAME " Running");

	currentProfile = DataLoader::defaultProfile;
	currentProfile->restart();

	while (running) {

		start = high_resolution_clock::now();

		if (messages.read()) {
			Message msg = messages.getMessage();
			LogDebug("Received message: task: " + Message::type2str(msg.getType()) + ", data: " + msg.toString());
			switch (msg.getType()) {

			case Message::Types::LoadProfile:
				if (not tryProfiles(msg.getData()))
					LogInfo("All requested profiles failed");
				break;

			case Message::Types::FinishLastProfile:
				if (not profiles.size()) {
#ifdef DEVELOP
					LogDebug("Cannot terminate the default profile.");
#endif
					break;
				}

				LogInfo("Profile " + currentProfile->getName() + " changed state to finishing.");
				terminateCurrentProfile();
				break;

			case Message::Types::FinishAllProfiles:
				if (profiles.size()) {
					terminateCurrentProfile();
					profiles.clear();
					currentProfile = DataLoader::defaultProfile;
					currentProfile->reset();
				}
				break;

			case Message::Types::SetElement: {
				if (msg.getData().size() != 3) {
					LogNotice("Invalid message ");
					break;
				}
				string
					name   = msg.getData()[0],
					color  = msg.getData()[1],
					filter = msg.getData()[2];
				if (not DataLoader::allElements.count(name)) {
					LogNotice("Unknown element " + name);
					break;
				}
				try {
					alwaysOnElements[name] = Element::Item{DataLoader::allElements.at(name), &Color::getColor(color), Color::str2filter(filter)};
				}
				catch (Error& e) {
					LogNotice(e.getMessage());
				}
				break;
			}

			case Message::Types::ClearElement:
				if (msg.getData().size() != 1) {
					LogNotice("Invalid message for " + Message::type2str(Message::Types::ClearElement));
					break;
				}
				if (alwaysOnElements.count(msg.getData()[0]))
					alwaysOnElements.erase(msg.getData()[0]);
				break;

			case Message::Types::ClearAllElements:
				alwaysOnElements.clear();
				break;

			case Message::Types::SetGroup:
				if (msg.getData().size() != 3) {
					LogNotice("Invalid message for " + Message::type2str(Message::Types::SetGroup));
					break;
				}
				if (not DataLoader::layout.count(msg.getData()[0])) {
					LogNotice("Unknown group " + msg.getData()[0]);
					break;
				}
				try {
					alwaysOnGroups[msg.getData()[0]] = Group::Item{
						&DataLoader::layout.at(msg.getData()[0]),
						&Color::getColor(msg.getData()[1]),
						Color::str2filter(msg.getData()[2])
					};
				}
				catch (Error& e) {
					LogNotice(e.getMessage());
				}
				break;

			case Message::Types::ClearGroup:
				if (msg.getData().size() != 1) {
					LogNotice("Unknown group for " + Message::type2str(Message::Types::ClearGroup));
					break;
				}
				if (alwaysOnGroups.count(msg.getData()[0]))
					alwaysOnGroups.erase(msg.getData()[0]);
				break;

			case Message::Types::ClearAllGroups:
				alwaysOnGroups.clear();
				break;

			case Message::Types::CraftProfile: {
				/*
				 * 0 target name
				 * 1 elements
				 * 2 groups
				 * 3 system
				 */
				if (msg.getData().size() != 4) {
					LogNotice("Invalid message for " + Message::type2str(Message::Types::ClearGroup));
					break;
				}
				// Try game profile.
				if (tryProfiles({msg.getData()[0]}))
					break;

				// Craft profile.
				Profile* profile = craftProfile(msg.getData()[3], msg.getData()[1], msg.getData()[2]);
				if (profile) {
					// Deactivate any overwrite.
					alwaysOnGroups.clear();
					alwaysOnElements.clear();
					DataLoader::controlledItems.clear();
					profiles.push_back(profile);
					currentProfile = profile;
					profile->restart();
					break;
				}
#ifdef DEVELOP
				else {
					LogDebug("Failed to craft profile with " + msg.getData()[3]);
				}
#endif
				// Load system.
				if (not tryProfiles({msg.getData()[3]}))
					LogInfo("All requested profiles failed");
				break;
			}

			// Other request that are not handled yet by ledspicerd.
			default:
				break;
			}
		}
		runCurrentProfile();
	}
	currentProfile = profiles[0];
	terminateCurrentProfile();
}

void Main::terminate() {
	running = false;
}

void Main::terminateCurrentProfile() {
	// Deactivate overwrites.
	alwaysOnGroups.clear();
	alwaysOnElements.clear();
	DataLoader::controlledItems.clear();
	currentProfile->terminate();

	// Wait for termination.
	while (currentProfile->isRunning()) {
		start = high_resolution_clock::now();
		currentProfile->runFrame();
		sendData();
	}
}

int main(int argc, char **argv) {

	// Process command line options.
	string
		commandline,
		profile,
		configFile = "";

	for (int i = 1; i < argc; i++) {

		commandline = argv[i];

		// Help text.
		if (commandline == "-h" or commandline == "--help") {
			cout <<
				PACKAGE_NAME " command line usage:\n"
				PACKAGE " <options>\n"
				"options:\n"
				"-c <conf> or --config <conf>\t\tUse an alternative configuration file\n"
				"-f or --foreground\t\t\tRun on foreground\n"
				"-d or --dump\t\t\t\tDump configuration files and quit\n"
				"-p <profile> or --profile <profile>\tDump a profile by name and quit\n"
				"-l or --leds\t\t\t\tTest LEDs.\n"
				"-e or --elements\t\t\tTest registered elements.\n"
				"-v or --version\t\t\t\tDisplay version information\n"
				"-h or --help\t\t\t\tDisplay this help screen.\n"
				"Data directory:    " PACKAGE_DATA_DIR "\n"
				"Actors directory:  " ACTORS_DIR "\n"
				"Devices directory: " DEVICES_DIR "\n"
				"Inputs directory:  " INPUTS_DIR "\n"
				"If -c or --config is not provided " PACKAGE_NAME " will use " CONFIG_FILE
				<< endl;
			return EXIT_SUCCESS;
		}

		// Version Text.
		if (commandline == "-v" or commandline == "--version") {
			cout
				<< endl <<
				PACKAGE_STRING " Copyright © 2018 - 2020 - Patricio A. Rossi (MeduZa)\n\n"
				"For more information visit <" PACKAGE_URL ">\n\n"
				"To report errors or bugs visit <" PACKAGE_BUGREPORT ">\n"
				PACKAGE_NAME " is free software under the GPL 3 license\n\n"
				"See the GNU General Public License for more details <http://www.gnu.org/licenses/>."
				<< endl;
			return EXIT_SUCCESS;
		}

		// Alternative configuration.
		if (commandline == "-c" or commandline == "--config") {
			configFile = argv[++i];
			continue;
		}

		// Dump configuration.
		if (commandline == "-d" or commandline == "--dump") {
			DataLoader::setMode(DataLoader::Modes::Dump);
			continue;
		}

		// Dump profile.
		if (commandline == "-p" or commandline == "--profile") {
			DataLoader::setMode(DataLoader::Modes::Profile);
			profile = argv[++i];
			continue;
		}

		// Test LEDs.
		if (commandline == "-l" or commandline == "--leds") {
			DataLoader::setMode(DataLoader::Modes::TestLed);
			continue;
		}

		// Test Elements
		if (commandline == "-e" or commandline == "--elements") {
			DataLoader::setMode(DataLoader::Modes::TestElement);
			continue;
		}

		// Force foreground.
		if (DataLoader::getMode() == DataLoader::Modes::Normal and (commandline == "-f" or commandline == "--foreground")) {
			DataLoader::setMode(DataLoader::Modes::Foreground);
			continue;
		}
	}

	Log::initialize(DataLoader::getMode() != DataLoader::Modes::Normal);

	if (configFile.empty())
		configFile = CONFIG_FILE;

	// Read Configuration.
	try {
		DataLoader config(configFile, "Configuration");
		config.readConfiguration();

#ifdef DEVELOP
		signal(SIGSEGV, signalHandler);
#endif
		signal(SIGTERM, signalHandler);
		signal(SIGQUIT, signalHandler);
		signal(SIGABRT, signalHandler);
		signal(SIGINT, signalHandler);
		signal(SIGCONT, SIG_IGN);
		signal(SIGSTOP, SIG_IGN);
		signal(SIGHUP, signalHandler);

		if (DataLoader::getMode() == DataLoader::Modes::Profile and DataLoader::defaultProfile->getName() != profile)
			DataLoader::defaultProfile = DataLoader::processProfile(profile);

#ifdef DEVELOP
	// force debug mode logging.
	Log::logToStdErr(DataLoader::getMode() != DataLoader::Modes::Normal);
	Log::setLogLevel(LOG_DEBUG);
#endif

		// Run mode.
		Main ledspicer;

		switch (DataLoader::getMode()) {
		case DataLoader::Modes::Profile:
			ledspicer.dumpProfile();
			break;
		case DataLoader::Modes::Dump:
			ledspicer.dumpConfiguration();
			break;

		case DataLoader::Modes::TestLed:
			ledspicer.testLeds();
			break;

		case DataLoader::Modes::TestElement:
			ledspicer.testElements();
			break;

		default:
			ledspicer.run();
		}
	}
	catch(Error& e) {
		LogError("Program terminated by error: " + e.getMessage());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

void Main::runCurrentProfile() {

	if (not currentProfile->isRunning()) {
		// Profiles are cached and reused when not running,
		// discard the current profile, pick and reset the previous.
#ifdef DEVELOP
		LogDebug("Profile " + currentProfile->getName() + " instance deleted");
#endif
		profiles.pop_back();
		currentProfile = profiles.size() ? profiles.back() : DataLoader::defaultProfile;

		// Do not run transition.
		currentProfile->reset();
		return;
	}

	// Reset elements.
	for (auto& eD : DataLoader::allElements)
		eD.second->setColor(currentProfile->getBackgroundColor());

	// Set always on groups from profile.
	for (auto& gE : currentProfile->getAlwaysOnGroups())
		gE.process();

	// Set always on elements from profile.
	for (auto& eE : currentProfile->getAlwaysOnElements())
		eE.process();

	currentProfile->runFrame();

	// Set always on groups from config.
	for (auto& gE : alwaysOnGroups)
		gE.second.process();

	// Set always on elements from config.
	for (auto& eE : alwaysOnElements)
		eE.second.process();

	// Set controlled items from input plugins.
	for (auto& item : DataLoader::controlledItems)
		item.second->process();

	sendData();
}

void Main::sendData() {
	// Send data.
	// TODO: need to test speed: single thread or running one thread per device.
	for (auto device : DataLoader::devices)
		device->packData();

	// Wait...
	wait(duration_cast<milliseconds>(high_resolution_clock::now() - start));
}
