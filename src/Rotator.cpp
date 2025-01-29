/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Rotator.cpp
 * @since     Aug 7, 2020
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2018 - 2025 Patricio A. Rossi (MeduZa)
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

#include "Rotator.hpp"

using namespace LEDSpicer;

umap<string, uint8_t> processPlayerNode(tinyxml2::XMLElement* restrictor) {
	umap<string, uint8_t> playerData;
	tinyxml2::XMLElement* element = restrictor->FirstChildElement(PLAYER_MAP);
	if (not element)
		return playerData;
	for (; element; element = element->NextSiblingElement(PLAYER_MAP)) {
		umap<string, string> playerAttr = XMLHelper::processNode(element);
		Utility::checkAttributes(REQUIRED_PARAM_MAP, playerAttr, PLAYER_MAP);
		uint8_t n = Utility::parseNumber(playerAttr["id"], "Invalid value for ID");
		if (not Utility::verifyValue(n, uint8_t(1), uint8_t(UINT8_MAX), false))
			throw Error("ID must be greater than 0");
		playerData.emplace(playerAttr["player"] + "_" + playerAttr["joystick"], n);
	}
	return playerData;
}

int main(int argc, char **argv) {

	string
		commandline,
		configFile = "";

	string resetWays;

	Log::initialize(true);

	umap<string, Restrictor::Ways> playersRequestedData, availablePlayersData;

	for (int i = 1; i < argc; i++) {

		commandline = argv[i];

		// Help text.
		if (commandline == "-h" or commandline == "--help") {
			cout <<
				"Rotator command line usage:\n"
				"rotator <options> player joystick ways\n"
				"You can repeat 'player joystick ways' as many times as needed:\n"
				" value for player: 1, 2, etc.\n"
				" value for joystick: 1, 2, etc.\n"
				" value for ways: 1, 2, strange2, vertical2, '3 (half4)', 4, 4x, 8, '5 (half8)', 16, 49, analog, mouse, rotary8, rotary12.\n"
				"Note: if your hardware does not support a specific ways the closes available one will be used.\n\n"
				"Examples:\n"
				"Rotate player 1 joystick 1 into 4 ways:\n"
				"rotator 1 1 4\n"
				"Rotate player 1 joystick 1 into 4 ways and player 2 joystick 1 into 4 ways:\n"
				"rotator 1 1 4 2 1 4\n"
				"Rotate player 1 joystick 1 into 2 ways and player 1 joystick 2 into 2 ways vertically:\n"
				"rotator 1 1 2 1 2 vertical2\n"
				"Options:\n"
				"-r <ways> or --reset <ways>  Reset all the restrictors to way.\n"
				"-c <conf> or --config <conf> Use an alternative configuration file.\n"
				"-v or --version              Display version information.\n"
				"-h or --help                 Display this help screen.\n"
				"If -c or --config is not provided rotator will use " CONFIG_FILE
				<< endl;
			return EXIT_SUCCESS;
		}

		// Version Text.
		if (commandline == "-v" or commandline == "--version") {
			cout
				<< endl <<
				"Rotator is part of " PACKAGE_STRING << endl <<
				PACKAGE_STRING " " COPYRIGHT "\n\n"
				"For more information visit <" PACKAGE_URL ">\n\n"
				"To report errors or bugs visit <" PACKAGE_BUGREPORT ">\n"
				PACKAGE_NAME " is free software under the GPL 3 license\n\n"
				"See the GNU General Public License for more details <http://www.gnu.org/licenses/>."
				<< endl;
			return EXIT_SUCCESS;
		}

		// Reset Only.
		if (commandline == "-r" or commandline == "--reset") {
			resetWays = argv[++i];
			continue;
		}

		// Alternative configuration.
		if (commandline == "-c" or commandline == "--config") {
			configFile = argv[++i];
			continue;
		}

		if ((i + 2) > argc) {
			LogError("Invalid player data");
			return EXIT_FAILURE;
		}
		playersRequestedData.emplace(commandline + "_" + argv[i + 1], Restrictor::str2ways(argv[i + 2]));
		i+=2;
	}

	if (playersRequestedData.empty() and resetWays.empty()) {
		LogError("Nothing to do");
		return EXIT_SUCCESS;
	}

	Log::initialize(true);

	if (configFile.empty())
		configFile = CONFIG_FILE;

	try {

		// Read Configuration.
		XMLHelper config(configFile, "Configuration");
		umap<string, string> configValues = XMLHelper::processNode(config.getRoot());

		// Set log level.
		if (configValues.count("logLevel"))
			Log::setLogLevel(Log::str2level(configValues["logLevel"]));

		// Process restrictors.
		tinyxml2::XMLElement* element = config.getRoot()->FirstChildElement(RESTRICTORS);
		if (not element)
			throw Error("No restrictors section detected.");

		element = element->FirstChildElement(RESTRICTOR);
		if (not element)
			throw Error("No restrictors found.");

		for (; element; element = element->NextSiblingElement(RESTRICTOR)) {
			umap<string, string> restrictorAttr(XMLHelper::processNode(element));
			umap<string, uint8_t> configPlayersData(std::move(processPlayerNode(element)));
			if (configPlayersData.empty())
				continue;
			Utility::checkAttributes(REQUIRED_PARAM_RESTRICTOR, restrictorAttr, RESTRICTOR);
			Restrictor* restrictor = nullptr;
			if (restrictorAttr["name"] == ULTRASTIK_NAME) {
				restrictor = new UltraStik360(restrictorAttr, configPlayersData);
			}
			else if (restrictorAttr["name"] == SERVOSTIK_NAME) {
				restrictor = new ServoStik(restrictorAttr, configPlayersData);
			}
			else if (restrictorAttr["name"] == GPWIZ49_NAME) {
				restrictor = new GPWiz49(restrictorAttr, configPlayersData);
			}
			else if (restrictorAttr["name"] == GPWIZ40ROTOX_NAME) {
				restrictor = new GPWiz40RotoX(restrictorAttr, configPlayersData);
			}
			else if (restrictorAttr["name"] == TOS428_NAME) {
				restrictor = new TOS428(restrictorAttr, configPlayersData);
			}
			else {
				LogError("Unknown hardware type " + restrictorAttr["name"]);
				continue;
			}

			if (configPlayersData.size() > restrictor->getMaxIds()) {
				LogDebug(restrictor->getFullName() + " supports only " + to_string(restrictor->getMaxIds()) + " devices");
			}

			for (const auto& pd : configPlayersData) {
				// Set Rotations.
				if (playersRequestedData.size() and playersRequestedData.count(pd.first)) {
					availablePlayersData.emplace(pd.first, playersRequestedData.at(pd.first));
				}
				// Reset all.
				else if (resetWays.size()) {
					availablePlayersData.emplace(pd.first, Restrictor::str2ways(resetWays));
				}
				else {
					LogDebug(Restrictor::getProfileStr(pd.first) + " not requested for " + restrictor->getFullName());
				}
			}

			if (availablePlayersData.empty()) {
				continue;
			}
			restrictor->initialize();
			restrictor->rotate(availablePlayersData);
			availablePlayersData.clear();
			restrictor->terminate();
			delete restrictor;
			std::this_thread::sleep_for(std::chrono::milliseconds(250));
		}

	}
	catch(Error& e) {
		LogError("Error: " + e.getMessage());
	}
	USB::closeSession();
	return EXIT_SUCCESS;
}
