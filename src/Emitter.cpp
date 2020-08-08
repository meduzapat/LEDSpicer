/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Emiter.cpp
 * @since     Jul 8, 2018
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

#include "Emitter.hpp"

using namespace LEDSpicer;

int main(int argc, char **argv) {

	string
		commandline,
		configFile = "",
		port;

	Message msg;

	bool craftProfile = false;

	for (int i = 1; i < argc; i++) {

		commandline = argv[i];

		// Help text.
		if (commandline == "-h" or commandline == "--help") {
			cout <<
				"Emitter command line usage:\n"
				"Emitter <options> <command>\n\n"
				"command:\n" <<
				Message::type2str(Message::Types::LoadProfile) <<
				" profile1 profile2 ... profileX  Attempts to load the first valid profile from a list of profiles.\n" <<
				Message::type2str(Message::Types::LoadProfileByEmulator) <<
				" rom platform  Attempts to load a profile based on the platform and the ROM name.\n" <<
				Message::type2str(Message::Types::FinishLastProfile) <<
				"                           Terminates the current profile (doens't affect the default).\n" <<
				Message::type2str(Message::Types::FinishAllProfiles) <<
				"                           Removes every profile (doens't affect the default).\n" <<
				Message::type2str(Message::Types::SetElement) <<
				" elementName color filter         Changes an element's background color until the profile ends.\n" <<
				Message::type2str(Message::Types::ClearElement) <<
				" elementName                    Removes an element's background color.\n" <<
				Message::type2str(Message::Types::ClearAllElements) <<
				"                            Removes all element's background color.\n" <<
				Message::type2str(Message::Types::SetGroup) <<
				" groupName color filter             Changes a group's background color until the profile ends.\n" <<
				Message::type2str(Message::Types::ClearGroup) <<
				" groupName                        Removes a group's background color.\n" <<
				Message::type2str(Message::Types::ClearAllGroups) <<
				"                              Removes all group's background color.\n" <<
				"options:\n"
				"-c <conf> or --config <conf> Use an alternative configuration file\n"
				"-v or --version              Display version information\n"
				"-h or --help                 Display this help screen.\n"
				"If -c or --config is not provided emitter will use " CONFIG_FILE
				<< endl;
			return EXIT_SUCCESS;
		}

		// Version Text.
		if (commandline == "-v" or commandline == "--version") {
			cout
				<< endl <<
				"Emitter is part of " PACKAGE_STRING << endl <<
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

		if (msg.getType() == Message::Types::Invalid) {
			try {
				msg.setType(Message::str2type(commandline));
				continue;
			}
			catch (Error& e) {
				LogError(e.getMessage());
				return EXIT_FAILURE;
			}
		}
		else {
			msg.addData(commandline);
		}
	}

	if (msg.getType() == Message::Types::Invalid) {
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

		// Set craft profile mode.
		if (configValues.count("craftProfile"))
			craftProfile = configValues["craftProfile"] == "true";

		// Port.
		if (configValues.count("port"))
			port = configValues["port"];
		else
			throw Error("Missing port attribute");

		// Check request.
		vector<string> data = msg.getData();
		if (msg.getType() == Message::Types::LoadProfileByEmulator) {
			if (data.size() < 2) {
				LogError("Error: Invalid request");
				return EXIT_FAILURE;
			}

			msg.reset();
			// arcades (mame and others)
			if (data[1] == "arcade") {
				GameRecord gd;
				try {
					gd = parseMame(data[0]);
				}
				catch (Error& e) {
					LogError("Error: " + e.getMessage());
					return EXIT_FAILURE;
				}
				if (craftProfile) {
					msg.setType(Message::Types::CraftProfile);
					msg.addData(gd.toString());
				}
				else {
					msg.setType(Message::Types::LoadProfile);
					msg.addData(string(data[1]).append("/").append(data[0]));
					msg.addData("P" + gd.players + "_B" + gd.playersData[0].buttons);
					msg.addData(gd.playersData[0].type + gd.players + "_B" + gd.playersData[0].buttons);
					msg.addData(data[1]);
				}
				// Rotate restrictors.
				gd.rotate();
			}
		}

		// Open connection and send message.
		Socks sock(LOCALHOST, port);
		bool r = sock.send(msg.toString());
		LogDebug("Message " + string(r ? "sent successfully: " : "failed to send: ") + msg.toString());
	}
	catch(Error& e) {
		LogError("Error: " + e.getMessage());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

GameRecord parseMame(const string& rom) {

	string
		output,
		cmd = "grep -w '" + rom + "' " + CONTROLLERS_FILE;
	LogDebug("running: " + cmd);

	std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
	if (not pipe)
		throw Error("Failed to read games data file");

	std::array<char, 128> buffer;
	while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
		output += buffer.data();

	if (not output.size())
		throw Error("Game " + rom + " not found.");


	tinyxml2::XMLDocument xml;
	if (xml.Parse(output.c_str(), output.size()) != tinyxml2::XML_SUCCESS)
		throw Error("Invalid games data file" + output);

	LogDebug("Game data: " + output);

	GameRecord tempData;

	tinyxml2::XMLElement* element = xml.RootElement();
	if (not element)
		throw Error("Missing input node for " + rom);

	umap<string, string> tempAttr = XMLHelper::processNode(element);

	if (not tempAttr.count(PLAYERS))
		throw Error("Missing players attribute for " + rom);

	tempData.players = tempAttr[PLAYERS];

	if (tempAttr.count(COINS))
		tempData.coins = tempAttr[COINS];

	element = element->FirstChildElement(CONTROL);
	if (not element)
		throw Error("Missing control section " + rom);

	for (; element; element = element->NextSiblingElement()) {
		tempAttr = XMLHelper::processNode(element);
		if (not tempAttr.count(TYPE))
			continue;

		PlayerData pd = processControllerNode(tempAttr);

		if ((tempAttr[TYPE] == "doublejoy" or tempAttr[TYPE] == "triplejoy") and tempAttr.count(WAYS "2"))
			pd.ways.push_back(tempAttr[WAYS "2"]);
		if (tempAttr[TYPE] == "triplejoy" and tempAttr.count(WAYS "3"))
			pd.ways.push_back(tempAttr[WAYS "3"]);
		tempData.playersData.push_back(pd);
	}
	return tempData;
}

PlayerData processControllerNode(umap<string, string>& tempAttr, string variant) {

	PlayerData p;

	if (tempAttr[TYPE] == "doublejoy" or tempAttr[TYPE] == "triplejoy") {
		p.type = "joy";
	}
	else if (tempAttr[TYPE] == "stick") {
		p.type = "joy";
		p.ways.push_back("analog");
	}
	else {
		p.type = tempAttr[TYPE];
	}

	if (tempAttr.count(WAYS + variant))
		p.ways.push_back(tempAttr[WAYS + variant]);

	if (tempAttr.count(PLAYER))
		p.player = tempAttr[PLAYER];
	else
		p.player = "1";

	if (tempAttr.count(BUTTONS))
		p.buttons = tempAttr[BUTTONS];

	return p;
}


