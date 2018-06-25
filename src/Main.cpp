/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file		Main.cpp
 * @since		Jun 6, 2018
 * @author		Patricio A. Rossi (MeduZa)
 * @copyright	Copyright © 2018 Patricio A. Rossi (MeduZa)
 */

#include "Main.hpp"

#ifdef DEVELOP
#include <execinfo.h>
#endif

using namespace LEDSpicer;

bool Main::running = false;

Main::Main(bool daemonize, const string& configFile, bool dump) {

	// TODO: remove me
	daemonize = false;

	Log::initialize(not daemonize);

	if (daemonize and not dump) {
		Log::debug("Daemonizing");
		if (daemon(0, 0) == -1) {
			throw Error("Unable to daemonize.");
		}
		Log::debug("Daemonized");
	}

	Log::debug("Reading configuration");
	DataLoader config(configFile, "Configuration");
	config.read();
	devices    = config.getDevices();
	layout     = config.getLayout();
	animations = config.getAnimations();
	defaultStateAnimation = config.isAnimation();
	defaultStateValue = config.getDefaultStateValue();

	if (dump)
		dumpConfiguration();
	else
		running = true;
}

Main::~Main() {
	ConnectionUSB::terminate();
	for (auto d : devices)
		delete d;
	for (auto a : animations)
		for (auto actor : a.second)
			delete actor;
}

void Main::run() {

	if (not running)
		return;

	Log::info("LEDSpicer Running");

}

void Main::terminate() {
	running = false;
}

void signalHandler(int sig) {

	if (sig == SIGTERM) {
		Log::info("Program terminated by signal");
		Main::terminate();
		return;
	}

	// display back trace
#ifdef DEVELOP
	void* array[10];
	size_t size = backtrace(array, 10);
	backtrace_symbols_fd(array, size, STDERR_FILENO);
#endif
	exit(EXIT_FAILURE);
}

void Main::dumpConfiguration() {
	cout << endl << "System Configuration:" << endl << "Colors:" << endl;
	Color::drawColors();
	cout << "Hardware:" << endl;
	for (auto d : devices) {
		d->drawHardwarePinMap();
	}
	cout << endl << "Layout:";
	for (auto group : layout) {
		cout << endl << "Group: " << group.first << " -> ";
		group.second.drawElements();
	}
	cout << endl << "Animations:" << endl;
	for (auto animation : animations) {
		cout << endl << animation.first << endl << "Actors:" << endl;
		for (auto actor : animation.second) {
			actor->drawConfig();
			cout << endl;
		}
	}
}

int main(int argc, char **argv) {

#ifdef DEVELOP
	signal(SIGSEGV, signalHandler);
#endif
	signal(SIGTERM, signalHandler);

	// Process command line options.
	string commandline, configFile = "";
	bool
		daemonize = true,
		dump      = true;

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
				"-v or --version\t\t\tDisplay version information\n"
				"-h or --help\t\t\tDisplay this help screen.\n"
				"Data directory: " PACKAGE_DATA_DIR "/\n"
				"If -c or --config is not provided " PACKAGE_NAME " will use " CONFIG_FILE
				<< endl;
			return EXIT_SUCCESS;
		}

		// Version Text.
		if (commandline == "-v" or commandline == "--version") {
			cout
				<< endl <<
				PACKAGE_NAME " Copyright © 2018 - Patricio A. Rossi (MeduZa)\n\n"
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
			dump = true;
			continue;
		}

		// Force foreground.
		if (commandline == "-f" or commandline == "--foreground") {
			daemonize = false;
			continue;
		}
	}

	if (configFile.empty())
		configFile = CONFIG_FILE;

	try {
		Main ledspicer(daemonize, configFile, dump);
		ledspicer.run();
	}
	catch(Error& e) {
		Log::error("Error: " + e.getMessage());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
