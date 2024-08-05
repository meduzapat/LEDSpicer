/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Main.cpp
 * @since     Jun 6, 2018
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

#include "Main.hpp"
#include <fcntl.h>

#ifdef DEVELOP
#include <execinfo.h>
#endif

using namespace LEDSpicer;

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
		cerr << PACKAGE_NAME " ended with signal " << sig << endl;
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

	currentProfile = DataLoader::defaultProfile = DataLoader::processProfile(DataLoader::defaultProfileName);
	currentProfile->restart();

	while (running) {

		// Frame begins.
		start = high_resolution_clock::now();

		if (not messages.read()) {
			runCurrentProfile();
			continue;
		}

		Message msg(messages.getMessage());
		LogDebug("Received message: Task: " + Message::type2str(msg.getType()) + "\nData: " + msg.toHumanString() + "\nFlags: " + Message::flag2str(msg.getFlags()));
		// Set global flags.
		if (msg.getType() == Message::Types::CraftProfile or msg.getType() == Message::Types::LoadProfile) {
			Utility::globalFlags = msg.getFlags();
		}

		switch (msg.getType()) {

		case Message::Types::LoadProfile:
			if (not tryProfiles(msg.getData())) {
				LogInfo("All requested profiles failed");
			}
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
			else {
#ifdef DEVELOP
				LogDebug("Cannot terminate the default profile.");
#endif
			}
			break;

		case Message::Types::SetElement: {
			if (msg.getData().size() < 1 or not DataLoader::allElements.count(msg.getData()[0])) {
				LogNotice("Invalid message for " + Message::type2str(msg.getType()));
				break;
			}

			if (msg.getData().size() == 1)
				msg.addData(DataLoader::allElements.at(msg.getData()[0])->getDefaultColor().getName());

			if (msg.getData().size() == 2)
				msg.addData("Normal");

			try {
				Profile::addTemporaryAlwaysOnElement(
					msg.getData()[0], Element::Item{
						DataLoader::allElements.at(msg.getData()[0]),
						&Color::getColor(msg.getData()[1]),
						Color::str2filter(msg.getData()[2])
					}
				);
			}
			catch (Error& e) {
				LogNotice(e.getMessage());
			}
			break;
		}

		case Message::Types::ClearElement:
			if (msg.getData().size() != 1) {
				LogNotice("Invalid element in message for " + Message::type2str(Message::Types::ClearElement));
				break;
			}
			Profile::removeTemporaryAlwaysOnElement(msg.getData()[0]);
			break;

		case Message::Types::ClearAllElements:
			Profile::removeTemporaryAlwaysOnElements();
			break;

		case Message::Types::SetGroup:
			if (msg.getData().size() < 1 or not DataLoader::layout.count(msg.getData()[0])) {
				LogNotice("Missing/Invalid group for " + Message::type2str(Message::Types::SetGroup));
				break;
			}

			if (msg.getData().size() == 1)
				msg.addData(DataLoader::layout.at(msg.getData()[0]).getDefaultColor().getName());

			if (msg.getData().size() == 2)
				msg.addData("Normal");

			try {
				Profile::addTemporaryAlwaysOnGroup(
					msg.getData()[0], Group::Item{
						&DataLoader::layout.at(msg.getData()[0]),
						&Color::getColor(msg.getData()[1]),
						Color::str2filter(msg.getData()[2])
					}
				);
			}
			catch (Error& e) {
				LogNotice(e.getMessage());
			}
			break;

		case Message::Types::ClearGroup:
			if (msg.getData().size() != 1 or not DataLoader::layout.count(msg.getData()[0])) {
				LogNotice("Unknown group for " + Message::type2str(Message::Types::ClearGroup));
				break;
			}
			Profile::removeTemporaryAlwaysOnGroup(msg.getData()[0]);
			break;

		case Message::Types::ClearAllGroups:
			Profile::removeTemporaryAlwaysOnGroups();
			break;

		case Message::Types::CraftProfile: {
			/*
			 * 0 target name
			 * 1 elements
			 * 2 groups
			 * 3 system
			 */
			if (msg.getData().size() != 4) {
				LogNotice("Invalid message for " + Message::type2str(Message::Types::CraftProfile));
				break;
			}
			Profile* profile = tryProfiles({msg.getData()[0]});
			if (not profile) {
				// Craft profile.
				profile = craftProfile(msg.getData()[3], msg.getData()[1], msg.getData()[2]);
				if (not profile) {
					// Try platform profile.
					profile = tryProfiles({msg.getData()[3]});
				}
			}
			break;
		}

		// Other request that are not handled yet by ledspicerd.
		default:
			break;
		}
	}
	currentProfile = DataLoader::defaultProfile;
	terminateCurrentProfile();
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
				PACKAGE_STRING " " COPYRIGHT "\n\n"
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

		signal(SIGSEGV, signalHandler);
		signal(SIGTERM, signalHandler);
		signal(SIGQUIT, signalHandler);
		signal(SIGABRT, signalHandler);
		signal(SIGINT,  signalHandler);
		signal(SIGCONT, reinterpret_cast<__sighandler_t>(1));
		signal(SIGSTOP, reinterpret_cast<__sighandler_t>(1));
		signal(SIGHUP,  signalHandler);

		if (DataLoader::getMode() == DataLoader::Modes::Profile)
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
		// Discard the current profile, pick and reset the previous.
		profiles.pop_back();
		currentProfile = profiles.size() ? profiles.back() : DataLoader::defaultProfile;

		// Do not run transition.
		currentProfile->reset();
		return;
	}

	// Reset elements.
	const Color& color(currentProfile->getBackgroundColor());
	for (auto& eD : DataLoader::allElements) {
		if (eD.second->isTimed())
			eD.second->checkTime();
		else
			eD.second->setColor(color);
	}

	currentProfile->runFrame();

	sendData();
}
