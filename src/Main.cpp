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

Main::Main(
	Modes mode,
	vector<Device*>& devices,
	umap<string, Group>& layout,
	Profile* defaultProfile,
	umap<string, Element*>& allElements
) :
	devices(std::move(devices)),
	layout(std::move(layout)),
	defaultProfile(defaultProfile),
	allElements(std::move(allElements))
{

	if (mode == Modes::Dump)
		return;

#ifndef DRY_RUN
	if (mode == Modes::Background) {
		Log::debug("Daemonizing");
		if (daemon(0, 0) == -1) {
			throw Error("Unable to daemonize.");
		}
		Log::debug("Daemonized");
	}
	for (auto device : this->devices)
		device->initialize();
#endif
	running = true;
}

Main::~Main() {

	delete defaultProfile;
	for (auto d : devices)
		delete d;
	ConnectionUSB::terminate();
}

void Main::run() {

	Log::info("LEDSpicer Running");

	while (running) {
		// 1 check queue for events.. <-|<-
			// run event ---------------| |
		// run default profile------------|

		for (auto device : devices)
			device->setLeds(0);
		defaultProfile->runFrame();

		// send data
		for (auto device : devices)
			device->transfer();

		Actor::advanceFrame();
	}
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
	cout  << endl << "Hardware:" << endl;
	for (auto d : devices)
		d->drawHardwarePinMap();
	cout << "Interval: " << (int)ConnectionUSB::getInterval() << "ms" << endl;
	cout << "Total Elements registered: " << (int)allElements.size() << endl;
	cout << endl << "Layout:";
	for (auto group : layout) {
		cout << endl << "Group: '" << group.first << "' with ";
		group.second.drawElements();
	}
	cout << endl << "Default Profile:" << endl;
	defaultProfile->drawConfig();
}

int main(int argc, char **argv) {

#ifdef DEVELOP
	signal(SIGSEGV, signalHandler);
#endif
	signal(SIGTERM, signalHandler);

	// Process command line options.
	string commandline, configFile = "";

	Main::Modes mode = Main::Modes::Normal;

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
			mode = Main::Modes::Dump;
			continue;
		}

		// Force foreground.
		if (commandline == "-f" or commandline == "--foreground") {
			if (mode == Main::Modes::Dump)
				continue;
			mode = Main::Modes::Background;
			continue;
		}
	}

#ifdef DEVELOP
	Log::initialize(true);
#else
	Log::initialize(mode == Main::Modes::Normal);
#endif

	if (configFile.empty())
		configFile = CONFIG_FILE;

	Log::debug("Reading configuration");

	try {
		DataLoader config(configFile, "Configuration");
		config.read();
		Main ledspicer(
			mode,
			config.getDevices(),
			config.getLayout(),
			config.processProfile(config.getDefaultProfile()),
			config.getElementList()
		);

		if (mode == Main::Modes::Dump) {
			ledspicer.dumpConfiguration();
			return EXIT_SUCCESS;
		}
		ledspicer.run();
	}
	catch(Error& e) {
		Log::error("Error: " + e.getMessage());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
