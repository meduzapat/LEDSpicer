/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Main.cpp
 * @since     Jun 6, 2018
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
		LogNotice(PROJECT_NAME " terminated by signal " + to_string(sig));
		Main::terminate();
		return;
	case SIGHUP:
		LogNotice(PROJECT_NAME " received reload configuration signal " + to_string(sig));
		// TODO: reload the profiles and restart everything?
		return;
	case SIGSEGV:
	case SIGILL:
	case SIGFPE:
	case SIGBUS:
		// Display back trace.
		cerr << PROJECT_NAME " ended with signal " << sig << endl;
#ifdef DEVELOP
		{
		void* array[10];
		size_t size = backtrace(array, 10);
		backtrace_symbols_fd(array, size, STDERR_FILENO);
		}
#endif
		exit(EXIT_FAILURE);
		break;
	default:
		// Log unexpected signals
		LogWarning(PROJECT_NAME " received unexpected signal: " + std::to_string(sig));
		break;
	}
}

void Main::run() {

	LogInfo(PROJECT_NAME " Running");

	// Run initial profile if any.
	Profile::defaultProfile->reset();
	Transition* from = DataLoader::getTransitionFromCache(Profile::defaultProfile);
	if (from) {
		LogInfo("Initializing with transition from profile " + Profile::defaultProfile->getName());
		// Create a temporary blank profile with default profile transition and run it.
		Profile* blank = new Profile("None", Color::Off);
		currentProfile = blank;
		currentProfile->reset();
		DataLoader::addTransitionIntoCache(blank, from);
		changeProfile(Profile::defaultProfile, false);
		// Remove blank
		DataLoader::removeTransitionFromCache(blank, false);
		delete blank;
	}
	else {
		currentProfile = Profile::defaultProfile;
	}
	while (running) {

		// Frame begins.
		start = high_resolution_clock::now();

		if (not messages.read()) {
			currentProfile->runFrame();
#ifdef BENCHMARK
			// Time message needs reset.
			timeMessage = {};
			timeAnimation = duration_cast<milliseconds>(high_resolution_clock::now() - start);
#endif
			sendData();
			continue;
		}

		// If set, will replace currentProfile.
		Profile* newProfile = nullptr;
		// If set, will store the profile on the stack.
		bool storeProfile   = false;

		Message msg(messages.getMessage());
		LogDebug("Received message: Task: " + Message::type2str(msg.getType()) + "\nData: " + msg.toHumanString() + "\nFlags: " + Message::flag2str(msg.getFlags()));
		// Set global flags.
		if (
			msg.getType() == Message::Types::CraftProfile or
			msg.getType() == Message::Types::LoadProfile  or
			msg.getType() == Message::Types::FinishLastProfile
		) {
			Utility::globalFlags = msg.getFlags();
		}

		switch (msg.getType()) {

		case Message::Types::LoadProfile:
			newProfile = tryProfiles(msg.getData());
			if (not newProfile) {
				LogInfo("All requested profiles failed");
				break;
			}
			storeProfile = true;
			break;

		case Message::Types::FinishLastProfile:
			if (not profiles.size()) break;
			LogInfo("Profile " + currentProfile->getName() + " terminated");
			profiles.pop_back();
			if (profiles.size())
				newProfile = profiles.back();
			else
				newProfile = Profile::defaultProfile;
			storeProfile = false;
			break;

		case Message::Types::FinishAllProfiles:
			if (not profiles.size()) break;
			profiles.clear();
			newProfile   = Profile::defaultProfile;
			storeProfile = false;
			break;

		case Message::Types::SetElement: {
			if (msg.getData().size() < 1 or not Element::allElements.exists(msg.getData()[0])) {
				LogNotice("Invalid message for " + Message::type2str(msg.getType()));
				break;
			}

			if (msg.getData().size() == 1)
				msg.addData(Element::allElements.at(msg.getData()[0])->getDefaultColor().getName());

			if (msg.getData().size() == 2)
				msg.addData("Normal");

			try {
				Profile::addTemporaryOnElement(
					msg.getData()[0], Element::Item{
						Element::allElements.at(msg.getData()[0]),
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
			Profile::removeTemporaryOnElement(msg.getData()[0]);
			break;

		case Message::Types::ClearAllElements:
			Profile::removeTemporaryOnElements();
			break;

		case Message::Types::SetGroup:
			if (msg.getData().size() < 1 or not Group::layout.exists(msg.getData()[0])) {
				LogNotice("Missing/Invalid group for " + Message::type2str(Message::Types::SetGroup));
				break;
			}

			if (msg.getData().size() == 1)
				msg.addData(Group::layout.at(msg.getData()[0]).getDefaultColor().getName());

			if (msg.getData().size() == 2)
				msg.addData("Normal");

			try {
				Profile::addTemporaryOnGroup(
					msg.getData()[0], Group::Item{
						&Group::layout.at(msg.getData()[0]),
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
			if (msg.getData().size() != 1 or not Group::layout.exists(msg.getData()[0])) {
				LogNotice("Unknown group for " + Message::type2str(Message::Types::ClearGroup));
				break;
			}
			Profile::removeTemporaryOnGroup(msg.getData()[0]);
			break;

		case Message::Types::ClearAllGroups:
			Profile::removeTemporaryOnGroups();
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
			newProfile   = tryProfiles({msg.getData()[0]});
			storeProfile = true;
			if (not newProfile) {
				// Craft profile.
				newProfile = craftProfile(msg.getData()[0], msg.getData()[3], msg.getData()[1], msg.getData()[2]);
				// Try platform profile.
				if (not newProfile) newProfile = tryProfiles({msg.getData()[3]});
			}
			break;
		}
		// Other request that are not handled yet by ledspicerd.
		default: break;
		}
#ifdef BENCHMARK
		timeMessage = duration_cast<milliseconds>(high_resolution_clock::now() - start);
#endif
		if (newProfile) {
			newProfile->enableAnimations(not (Utility::globalFlags & FLAG_NO_ANIMATIONS));
			newProfile->enableInputs(not (Utility::globalFlags & FLAG_NO_INPUTS));
			changeProfile(newProfile, storeProfile);
		}
	}
	// Terminate execution with the ending transition.
	changeProfile(nullptr, false);
}

void Main::terminate() {
	running = false;
}

int main(int argc, char **argv) {

	// Process command line options.
	string
		commandline,
		profile,
		configFile = "",
		projectDir = "",
		project    = "";

	for (int i = 1; i < argc; i++) {

		commandline = argv[i];

		// Help text.
		if (commandline == "-h" or commandline == "--help") {
			cout <<
				PROJECT_NAME " command line usage:\n"
				"ledspicer <options>\n"
				"options:\n"
				"-c <conf> or --config <conf>\t\tUse an alternative configuration file\n"
				"-f or --foreground\t\t\tRun on foreground\n"
				"-d or --dump\t\t\t\tDump configuration files and quit\n"
				"-p <profile> or --profile <profile>\tDump a profile by name and quit\n"
				"-j <project> or --project <project>\tReplace the default project (or set if none)\n"
				"-J <path> or --projects-dir <path>\tOverride projects base directory\n"
				"-l or --leds\t\t\t\tTest LEDs.\n"
				"-e or --elements\t\t\tTest registered elements.\n"
				"-v or --version\t\t\t\tDisplay version information\n"
				"-h or --help\t\t\t\tDisplay this help screen.\n"
				"Data dir:     " PROJECT_DATA_DIR "\n"
				"Devices dir:  " DEVICES_DIR "\n"
				"Projects dir: " << Utility::getConfigDir() << PROJECTS_DIR "\n"
				"Please note that if the program runs as different user id, the projects dir will be different\n"
				"If -c or --config is not provided " PROJECT_NAME " will use " CONFIG_FILE
				<< endl;
			return EXIT_SUCCESS;
		}

		// Version Text.
		if (commandline == "-v" or commandline == "--version") {
			cout
				<< endl <<
				PROJECT_NAME " " PROJECT_VERSION " " COPYRIGHT "\n\n"
				"For more information visit <" PROJECT_SITE ">\n\n"
				"To report errors or bugs visit <" PROJECT_BUGREPORT ">\n"
				PROJECT_NAME " is free software under the GPL 3 license\n\n"
				"See the GNU General Public License for more details <http://www.gnu.org/licenses/>"
				<< endl;
			return EXIT_SUCCESS;
		}

		// Alternative configuration.
		if (commandline == "-c" or commandline == "--config") {
			configFile = argv[++i];
			continue;
		}

		// Select project.
		if (commandline == "-j" or commandline == "--project") {
			project = argv[++i];
			continue;
		}

		// Override projects base directory.
		if (commandline == "-J" or commandline == "--projects-dir") {
			projectDir = argv[++i];
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
			if (profile.empty()) {
				cerr
					<< "A profile name must be provided with -p or --profile option"
					<< endl;
				return EXIT_FAILURE;
			}
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

	commandline.clear();

	Log::initialize(DataLoader::getMode() != DataLoader::Modes::Normal);

	if (configFile.empty()) configFile = CONFIG_FILE;
	// seed the random here, because loader uses it.
	std::srand(time(nullptr));

	try {

		signal(SIGSEGV, signalHandler);
		signal(SIGILL,  signalHandler);
		signal(SIGFPE,  signalHandler);
		signal(SIGBUS,  signalHandler);
		signal(SIGTERM, signalHandler);
		signal(SIGQUIT, signalHandler);
		signal(SIGABRT, signalHandler);
		signal(SIGINT,  signalHandler);
		signal(SIGHUP,  signalHandler);

		// Ignored signals
		signal(SIGPIPE, reinterpret_cast<__sighandler_t>(1));

		// Read Configuration.
		DataLoader config(configFile, "Configuration");
		config.readConfiguration(projectDir, project, profile);
		projectDir.clear();
		project.clear();
		profile.clear();
		configFile.clear();

#ifdef DEVELOP
	// force debug mode logging.
	Log::logToStdTerm(DataLoader::getMode() != DataLoader::Modes::Normal);
	Log::setLogLevel(LOG_DEBUG);
#endif

		// Instantiate the main program.
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
		LogError("Program terminated by error: " + e.getMessage() + (Log::isLogging(LOG_DEBUG) ? "" : "\nFor more details set debug mode in the config"));
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

Profile* Main::tryProfiles(const vector<string>& data) {
	// Reload (used for debugging) ignores the cache and rebuilds the profile in place.
	const bool reload {(Utility::globalFlags & FLAG_FORCE_RELOAD) != 0};
	for (const auto& profileName : data) {
		LogDebug("Attempting to load profile: " + profileName);
		try {
			Profile* old {DataLoader::getProfileFromCache(profileName)};
			if (old and not reload) {
				LogDebug("Profile " + profileName + " from cache");
				return old;
			}
			Profile* profile {DataLoader::processProfile(profileName)};
			replaceProfileReferences(old, profile);
			return profile;
		}
		catch (Error& e) {
			LogDebug("Profile failed: " + e.getMessage());
			continue;
		}
	}
	return nullptr;
}

void Main::replaceProfileReferences(Profile* old, Profile* neu) {
	if (not old or old == neu) return;
	DataLoader::removeTransitionFromCache(old, true);
	if (old == Profile::defaultProfile) Profile::defaultProfile = neu;
	if (old == currentProfile)          currentProfile = neu;
	// Update all stack copies before freeing the replaced profile.
	for (auto& p : profiles) if (p == old) p = neu;
	delete old;
}

Profile* Main::craftProfile(const string& name, const string& platform, const string& elements, const string& groups) {

	// Crafted profiles are cached per game; reload rebuilds in place.
	const string cacheKey {EMPTY_PROFILE + name};
	const bool reload {(Utility::globalFlags & FLAG_FORCE_RELOAD) != 0};
	try {
		Profile* old {DataLoader::getProfileFromCache(cacheKey)};
		if (old and not reload) {
			LogDebug("Profile " + cacheKey + " from cache");
			return old;
		}

		LogDebug("Crafting " + cacheKey + " from " EMPTY_PROFILE + platform);
		Profile* profile {DataLoader::processProfile(EMPTY_PROFILE + platform, cacheKey)};

		// Add elements.
		LogDebug("Adding elements");
		for (string& n : Utility::explode(elements, FIELD_SEPARATOR)) {
			const Color* col = nullptr;
			auto parts = Utility::explode(n, GROUP_SEPARATOR);
			n = parts[0];
			if (not Element::allElements.exists(n)) {
				LogDebug("Unknown element " + n);
				continue;
			}
			if (parts.size() == 2 and Color::hasColor(parts[1]))
				col = &Color::getColor(parts[1]);
			else
				col = &Element::allElements.at(n)->getDefaultColor();

			LogDebug("Using element " + n + " color " + col->getName());
			profile->addAlwaysOnElement(Element::allElements.at(n), *col, Color::Filters::Normal);
		}

		// Add Animations.
		LogDebug("Adding Animations");
		for (string& n : Utility::explode(groups, FIELD_SEPARATOR)) {
			LogDebug("Loading animation " + n);
			try {
				profile->addAnimation(DataLoader::processAnimation(n));
			}
			catch (...) {
				LogDebug(n + " failed");
				continue;
			}
		}

		// Add Inputs.
		LogDebug("Adding Inputs");
		for (string& n : Utility::explode(groups, FIELD_SEPARATOR)) {
			LogDebug("Loading input " + n);
			try {
				DataLoader::processInput(profile, n);
			}
			catch (...) {
				LogDebug(n + " failed");
				continue;
			}
		}

		replaceProfileReferences(old, profile);
		return profile;
	}
	catch (Error& e) {
		LogNotice(e.getMessage());
		return nullptr;
	}
}

void Main::changeProfile(Profile* to, bool store) {
	// TODO add benchmark timers
	// If there is a profile and replace flag, replace current profile.
	bool replace {profiles.size() and (Utility::globalFlags & FLAG_REPLACE)};
	if (to) {
		LogInfo((
			(Utility::globalFlags & FLAG_FORCE_RELOAD) ? "Reloading profile " :
			replace                                    ? "Replacing profile " :
			                                             "Changing profile ")
			+ currentProfile->getName() + " with " + to->getName());
	}
	else {
		LogInfo("Terminating Profile " + currentProfile->getName());
	}

	if (currentProfile != to) currentProfile->stopInputs();

	Transition* transition = (currentProfile != to) ? DataLoader::getTransitionFromCache(to) : nullptr;

	if (transition and not (Utility::globalFlags & FLAG_NO_TRANSITIONS)) {

		transition->activate(currentProfile, to);
		// Run transition.
		while (true) {
			start = high_resolution_clock::now();
			if (not transition->run()) break;
			sendData();
		}
		transition->deactivate();
	}
	else {
		if (to) to->reset();
	}

	// Stack maintenance (RELOAD already refreshed every matching instance from disk during fetch):
	//  - REPLACE swaps the top with the new profile, unless the top is already the same profile;
	//  - otherwise the load is pushed onto the history stack.
	if (store) {
		if (replace) {
			if (profiles.back() != to) {
				profiles.pop_back();
				profiles.push_back(to);
			}
		}
		else profiles.push_back(to);
	}
	currentProfile = to;
	if (to and not (Utility::globalFlags & FLAG_NO_INPUTS)) to->startInputs();
	Utility::globalFlags = 0;
}
