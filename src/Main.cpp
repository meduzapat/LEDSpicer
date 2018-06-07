/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file		Main.cpp
 * @since		Jun 6, 2018
 * @author		Patricio A. Rossi (MeduZa)
 * @copyright	Copyright © 2018 Patricio A. Rossi (MeduZa)
 */

#include "Main.hpp"

using namespace LEDSpicer;

Main::Main(bool daemonize, const string& config) {

	if (daemonize) {
		LOG(Log::DEBUG, "Daemonizing");
		if (daemon(0, 0) == -1) {
			throw Error("Unable to daemonize.");
		}
		LOG(Log::DEBUG, "Daemonized");
	}
}

Main::~Main() {
	// TODO Auto-generated destructor stub
}

void Main::run() {

}

void Main::terminate() {

}

void signalHandler(int sig) {

	if (sig == SIGTERM) {
		cout << "Program terminated by signal" << endl;
		Main::terminate();
		return;
	}

	// display backtrace
#ifdef DEVELOP
	void* array[10];
	size_t size = backtrace(array, 10);
	backtrace_symbols_fd(array, size, STDERR_FILENO);
#endif
	exit(EXIT_FAILURE);
}

int main(int argc, char **argv) {

#ifdef DEVELOP
	signal(SIGSEGV, signalHandler);
#endif
	signal(SIGTERM, signalHandler);

	// Process command line options.
	string commandline, configFile = "";
	bool daemonize = true;

	for (int i = 1; i < argc; i++) {

		commandline = argv[i];

		// Help text.
		if (commandline == "-h" or commandline == "--help") {
			cout <<
				PACKAGE_NAME " command line usage:\n"
				PACKAGE_TARNAME " <options>\n"
				"options:\n"
				"-c <conf> or --config <conf>\tUse an alternative configuration file\n"
				"-f or --foreground\t\tRun on foreground\n"
				"-v or --version\t\t\tDisplay version information\n"
				"-h or --help\t\t\tDisplay this help screen.\n"
				"If a configuration file is not provided " PACKAGE_NAME " will use " CONFIG_FILE
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

		// Force foreground.
		if (commandline == "-f" or commandline == "--foreground") {
			daemonize = false;
			continue;
		}
	}

	if (configFile.empty())
		configFile = CONFIG_FILE;

	Main ledspicer(daemonize, configFile);
	try {
		ledspicer.run();
	}
	catch(Error& e) {
		cout << "Error: " + e.getMessage() << endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
