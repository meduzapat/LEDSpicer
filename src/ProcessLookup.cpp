/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      ProcessLookup.cpp
 * @since     Aug 7, 2020
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

#include "ProcessLookup.hpp"

using namespace LEDSpicer;

struct Map {
	string
		system = "arcade",
		id     = "";
	uint8_t parameterPos = 0;
};

Map* currentMap = nullptr;

umap<string, Map> maps;

void callEmitter(const string& rom, const string& system = "") {
	string parameters(currentConfigFile != CONFIG_FILE ? "-c \"" + currentConfigFile + "\"" : "");
	string command;
	if (rom.empty())
		command = "emitter FinishLastProfile " + parameters;
	else
		command = "emitter LoadProfileByEmulator " + rom + " " + system + " " + parameters;
	LogDebug("Running: " + command);
	if (std::system(command.c_str()) != EXIT_SUCCESS) {
		LogWarning("Failed to execute " + command);
	}
}

#ifdef MiSTer

string lastRunning = "";

/*
 * MiSTer FPGA runs a process to control the menu and the load of cores.
 * That control is always there, it only changes the parameters.
 */

string getMiSTerName(const string& path) {
	string data;
	std::ifstream mraFile;
	mraFile.open(path.c_str(), std::ios::in);
	if (not mraFile.is_open())
		return "";
	std::size_t pos = 0;
	while (not mraFile.eof()) {
		std::getline(mraFile, data);
		pos = data.find("<setname>");
		if (pos == string::npos)
			continue;
		pos += 9;
		data = data.substr(pos , data.find("</") - pos);
		break;
	}
	mraFile.close();
	return data;
}

void checkIfProcessIsRunning() {

	// Read command line.
	string data = PROC_DIRECTORY + currentMap->id + CMDLINE;
	std::ifstream cmdFile;
	cmdFile.open(data.c_str(), std::ios::in);
	if (not cmdFile.is_open())
		throw Error("cannot find " MISTER_BINARY);
	std::getline(cmdFile, data);
	cmdFile.close();

	if (data.empty())
		return;
	vector<string> parts = Utility::explode(data, '\0');
	parts.pop_back();
	if (parts.empty())
		return;

	// parameterPos != 0 core running.
	if (currentMap->parameterPos) {

		// Core running.
		if (lastRunning == data)
			return;

		// Core ended.
		callEmitter("");
		currentMap->parameterPos = 0;

		// Is back to menu?
		if (parts.size() > 1 and parts.at(1) == "menu.rbf") {
			LogDebug("Core ended");
			lastRunning = "";
			return;
		}

		// Core is running but is not the same than before, different core loaded.
		LogDebug("Different Core loaded.");
		lastRunning = data;
	}

	// Detect core.
	string name;
	switch (parts.size()) {

	/*
	 * size:
	 * 2 other
	 * 3 arcade
	 */
	case 2:
		// Detect menu.
		if (parts.at(1) == "menu.rbf")
			return;
		// detect name
		parts = Utility::explode(parts[1], '/');
		parts = Utility::explode(parts.back(), '_');
		LogDebug("Non Arcade core detected");
		callEmitter("unknownGame", parts.front());
		currentMap->parameterPos = 1;
		break;
	case 3:
		LogDebug("Arcade core detected");
		callEmitter(getMiSTerName(parts[2]), "arcade");
		currentMap->parameterPos = 1;
		break;
	}
}
#else
void checkIfProcessIsRunning() {
	string file = PROC_DIRECTORY + currentMap->id;
	std::ifstream cmdFile;
	cmdFile.open(file.c_str(), std::ios::in);
	if (not cmdFile.is_open()) {
		LogDebug("Process " + currentMap->id + " ended");
		currentMap->id = "";
		currentMap = nullptr;
		callEmitter("");
		return;
	}
	cmdFile.close();
}
#endif

void findRunningProcess() {

	umap<string, string> processList;
	dirent* dirEntity = nullptr;

	DIR* dir = opendir(PROC_DIRECTORY) ;
	if (not dir)
		throw Error("Couldn't open the " PROC_DIRECTORY " directory");

	// Loop while not NULL
	while ((dirEntity = readdir(dir))) {

		if (dirEntity->d_type != DT_DIR)
			continue;

		if (dirEntity->d_name[0] < '0' || dirEntity->d_name[0] > '9')
			continue;

		// Read command line.
		string file = PROC_DIRECTORY;
		file.append(dirEntity->d_name);
		file.append(CMDLINE);
		std::ifstream cmdFile;
		cmdFile.open(file.c_str(), std::ios::in);
		if (not cmdFile.is_open())
			continue;
		std::getline(cmdFile, file);
		cmdFile.close();
		if (file.empty())
			continue;

		// Process command line.
		vector<string> parts = Utility::explode(file, '\0');
		if (parts.size() < 2)
			continue;
		string name = Utility::explode(parts[0], '/').back();
#ifdef MiSTer
		if (name == MISTER_BINARY) {
			currentMap->id = dirEntity->d_name;
			break;
		}
#else
		for (auto& m : maps) {
			if (string(m.first) == name) {
				LogDebug("Found process " + name + " with ID " + dirEntity->d_name);
				maps.at(m.first).id = dirEntity->d_name;
				currentMap = &m.second;
				parts = Utility::explode(parts[1], ' ');
				if (parts.size() > m.second.parameterPos) {
					callEmitter(parts[m.second.parameterPos], m.second.system);
				}
				else {
					LogWarning("Parameter not found at " + to_string(m.second.parameterPos));
				}
				break;
			}
		}
#endif
	}
	closedir(dir);
}

void signalHandler(int sig) {

	switch (sig) {
	case SIGTERM:
	case SIGABRT:
	case SIGQUIT:
	case SIGINT:
		LogNotice("processLookup terminated by signal");
		running = false;
		return;
	case SIGHUP:
		LogNotice("processLookup received re load configuration signal");
		// TODO: reload the profiles and restart everything?
		return;
	case SIGSEGV:
	case SIGILL:
		// Display back trace.
		exit(EXIT_FAILURE);
		break;
	}
}

int main(int argc, char **argv) {

	signal(SIGTERM, signalHandler);
	signal(SIGQUIT, signalHandler);
	signal(SIGABRT, signalHandler);
	signal(SIGINT,  signalHandler);
	signal(SIGCONT, SIG_IGN);
	signal(SIGSTOP, SIG_IGN);
	signal(SIGHUP,  signalHandler);
	string
		commandline,
		configFile   = "";

	milliseconds waitTime;

	bool demonize = true;

	Log::initialize(true);

#ifdef MiSTer
	LogDebug("Compiled for " MISTER_BINARY);
#endif

	for (int i = 1; i < argc; i++) {

		commandline = argv[i];

		// Help text.
		if (commandline == "-h" or commandline == "--help") {
			cout <<
				"processLookup command line usage:\n"
				"Options:\n"
				"-f or --foreground           Run on foreground\n"
				"-c <conf> or --config <conf> Use an alternative configuration file\n"
				"-v or --version              Display version information\n"
				"-h or --help                 Display this help screen.\n"
				"If -c or --config is not provided rotator will use " CONFIG_FILE
				<< endl;
			return EXIT_SUCCESS;
		}

		// Version Text.
		if (commandline == "-v" or commandline == "--version") {
			cout
				<< endl <<
				"processLookup is part of " PACKAGE_STRING << endl <<
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

		// Force foreground.
		if (commandline == "-f" or commandline == "--foreground") {
			demonize = false;
			continue;
		}
	}

	Log::initialize(true);

	if (configFile.empty())
		configFile = CONFIG_FILE;

	currentConfigFile = configFile;

	try {

		if (demonize) {
			LogDebug("Daemonizing");
			if (daemon(0, 0) == -1)
				throw Error("Unable to daemonize.");
			LogDebug("Daemonized");
		}

		// Read Configuration.
		XMLHelper config(configFile, "Configuration");
		umap<string, string> configValues = XMLHelper::processNode(config.getRoot());

		// Set log level.
		if (configValues.count("logLevel"))
			Log::setLogLevel(Log::str2level(configValues["logLevel"]));

		// Process main node.
		tinyxml2::XMLElement* elementXML = config.getRoot()->FirstChildElement(NODE_MAIN_PROCESS);
		if (not elementXML)
			throw Error("Missing configuration");

		configValues = XMLHelper::processNode(elementXML);

		// Set read time.
		int waitTimeNum = configValues.count(PARAM_MILLISECONDS) ? Utility::parseNumber(configValues[PARAM_MILLISECONDS], "Invalid value for " PARAM_MILLISECONDS) : 1000;
		if (waitTimeNum == 0)
			throw LEDError(PARAM_MILLISECONDS " should be a number bigger than 0");
		waitTime = milliseconds(waitTimeNum);

#ifdef MiSTer
		maps.emplace(MISTER_BINARY , Map{"arcade", "", 0});
		currentMap = &maps.at(MISTER_BINARY);
		findRunningProcess();
		if (currentMap->id.empty())
			throw Error(MISTER_BINARY " not found");
		LogDebug("Found " MISTER_BINARY " with ID " + currentMap->id);
#else
		// Check Maps.
		elementXML = elementXML->FirstChildElement(NODE_MAP);
		if (not elementXML)
			throw Error("No map found in the config");

		for (; elementXML; elementXML = elementXML->NextSiblingElement(NODE_MAP)) {
			configValues = XMLHelper::processNode(elementXML);
			// Set process name.
			if (not configValues.count(PARAM_PROCESS_NAME)) {
				LogWarning("Missing " PARAM_PROCESS_NAME);
				continue;
			}
			maps.emplace(
				configValues[PARAM_PROCESS_NAME] , Map{
					configValues.count(PARAM_SYSTEM) ? configValues[PARAM_SYSTEM] : "arcade",
					"",
					static_cast<uint8_t>(configValues.count(PARAM_PROCESS_POS) ? Utility::parseNumber(configValues[PARAM_PROCESS_POS], "") : 0)
				}
			);
		}
		if (not maps.size())
			throw Error("No maps found");
#endif
	}
	catch(Error& e) {
		LogError("Error: " + e.getMessage());
		return EXIT_FAILURE;
	}

	while (running) {
#ifdef MiSTer
		checkIfProcessIsRunning();
#else
		// check process list.
		if (not currentMap)
			findRunningProcess();
		else
			checkIfProcessIsRunning();
#endif
		std::this_thread::sleep_for(waitTime);
	}
	return EXIT_SUCCESS;
}
