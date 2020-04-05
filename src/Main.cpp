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
		// display back trace
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
				if (profiles.size() == 1) {
					LogDebug("Cannot terminate the default profile.");
					break;
				}
				// TODO: this starts to been annoying, I will finish them anyway.
				if (currentProfile->isTransiting()) {
					LogInfo("Profile " + currentProfile->getName() + " is finishing, try later.");
					break;
				}
				LogInfo("Profile " + currentProfile->getName() + " changed state to finishing.");
				currentProfile->terminate();
				// Deactivate overwrites.
				alwaysOnGroups.clear();
				alwaysOnElements.clear();
				DataLoader::controlledGroups.clear();
				DataLoader::controlledElements.clear();
				break;

			case Message::Types::FinishAllProfiles:
				if (profiles.size() > 1) {
					currentProfile->terminate();
					// Deactivate overwrites.
					alwaysOnGroups.clear();
					alwaysOnElements.clear();
					DataLoader::controlledGroups.clear();
					DataLoader::controlledElements.clear();
					profiles.clear();
					currentProfile = DataLoader::defaultProfile;
					profiles.push_back(currentProfile);
				}
				break;

			case Message::Types::SetElement:
				if (msg.getData().size() != 3) {
					LogNotice("Invalid message ");
					break;
				}
				if (not DataLoader::allElements.count(msg.getData()[0])) {
					LogNotice("Unknown element " + msg.getData()[0]);
					break;
				}
				try {
					if (alwaysOnElements.count(msg.getData()[0]))
							alwaysOnElements[msg.getData()[0]] = Element::Item{
							DataLoader::allElements[msg.getData()[0]],
							&Color::getColor(msg.getData()[1]),
							Color::str2filter(msg.getData()[2])
						};
					else
						alwaysOnElements.emplace(msg.getData()[0], Element::Item{
							DataLoader::allElements[msg.getData()[0]],
							&Color::getColor(msg.getData()[1]),
							Color::str2filter(msg.getData()[2])
						});
				}
				catch (Error& e) {
					LogNotice(e.getMessage());
				}
				break;

			case Message::Types::ClearElement:
				if (msg.getData().size() != 1) {
					LogNotice("Invalid message " + msg.getData()[0]);
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
					LogNotice("Invalid message ");
					break;
				}
				if (not DataLoader::layout.count(msg.getData()[0])) {
					LogNotice("Unknown group " + msg.getData()[0]);
					break;
				}
				try {
					if (alwaysOnGroups.count(msg.getData()[0]))
						alwaysOnGroups[msg.getData()[0]] = Group::Item{
							&DataLoader::layout[msg.getData()[0]],
							&Color::getColor(msg.getData()[1]),
							Color::str2filter(msg.getData()[2])
						};
					else
						alwaysOnGroups.emplace(msg.getData()[0], Group::Item{
							&DataLoader::layout[msg.getData()[0]],
							&Color::getColor(msg.getData()[1]),
							Color::str2filter(msg.getData()[2])
						});
				}
				catch (Error& e) {
					LogNotice(e.getMessage());
				}
				break;

			case Message::Types::ClearGroup:
				if (msg.getData().size() != 1) {
					LogNotice("Unknown group " + msg.getData()[0]);
					break;
				}
				if (alwaysOnGroups.count(msg.getData()[0]))
					alwaysOnGroups.erase(msg.getData()[0]);
				break;

			case Message::Types::ClearAllGroups:
				alwaysOnGroups.clear();
				break;

			// Other request that are not handled yet by ledspicerd.
			default:
				break;
			}
		}
		runCurrentProfile();
	}
	currentProfile = profiles[0];
	currentProfile->terminate();

	// Wait for termination.
	while (currentProfile->isRunning()) {
		start = high_resolution_clock::now();
		currentProfile->runFrame();
		sendData();
	}
}

void Main::terminate() {
	running = false;
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
				"-c <conf> or --config <conf>\tUse an alternative configuration file\n"
				"-f or --foreground\t\tRun on foreground\n"
				"-d or --dump\t\t\tDump configuration files and quit\n"
				"-p <profile> or --profile <profile>\t\t\tDump a profile by name and quit\n"
				"-l or --leds\t\t\tTest LEDs.\n"
				"-e or --elements\t\tTest registered elements.\n"
				"-v or --version\t\t\tDisplay version information\n"
				"-h or --help\t\t\tDisplay this help screen.\n"
				"Data directory: " PACKAGE_DATA_DIR "\n"
				"Actors directory: " ACTORS_DIR "\n"
				"Devices directory: " DEVICES_DIR "\n"
				"Inputs directory: " INPUTS_DIR "\n"
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
		// Profiles are cached and reused when not running, discard the current profile, pick and reset the previous.
		profiles.pop_back();
		currentProfile = profiles.back();
		currentProfile->reset();
		return;
	}

	// Reset elements.
	for (auto& eD : DataLoader::allElements)
		eD.second->setColor(currentProfile->getBackgroundColor());

	// Set always on groups from profile.
	for (auto& gE : currentProfile->getAlwaysOnGroups())
		for (auto eE : gE.group->getElements())
			eE->setColor(*gE.color, gE.filter);

	// Set always on elements from profile.
	for (auto& eE : currentProfile->getAlwaysOnElements())
		eE.element->setColor(*eE.color, eE.filter);

	currentProfile->runFrame();

	// Set always on groups from config.
	for (auto& gE : alwaysOnGroups)
		for (auto eE : gE.second.group->getElements())
			eE->setColor(*gE.second.color, gE.second.filter);

	// Set always on elements from config.
	for (auto& eE : alwaysOnElements)
		eE.second.element->setColor(*eE.second.color, eE.second.filter);

	// Set controlled groups from input plugins.
	for (auto& gE : DataLoader::controlledGroups)
		for (auto eE : gE.second->group->getElements())
			eE->setColor(*gE.second->color, gE.second->filter);

	// Set controlled elements from input plugins.
	for (auto& eE : DataLoader::controlledElements)
		eE.second->element->setColor(*eE.second->color, eE.second->filter);

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
