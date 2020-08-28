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

	vector<string> dataSource;

	bool
		craftProfile = false,
		rotate       = false,
		useColors    = false;


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

		// Set use colors file.
		if (configValues.count("colorsFile"))
			useColors = configValues["colorsFile"] == "true";

		// Port.
		if (configValues.count("port"))
			port = configValues["port"];
		else
			throw Error("Missing port attribute");

		// Check restrictors.
		tinyxml2::XMLElement* xmlElement = config.getRoot()->FirstChildElement("restrictors");
		if (xmlElement and xmlElement->FirstChildElement("restrictor"))
			rotate = true;

		// Check request.
		vector<string> data = msg.getData();
		if (msg.getType() == Message::Types::LoadProfileByEmulator) {
			// param 1 is rom, param 2 is system.
			if (data.size() < 2) {
				LogError("Error: Invalid request");
				return EXIT_FAILURE;
			}

			msg.reset();
			msg.setType(Message::Types::LoadProfile);
			msg.addData(string(data[1]).append("/").append(data[0]));
			// Arcades (mame and others).
			if (data[1] == ARCADE_SYSTEM) {

				// Set data source.
				if (configValues.count(PARAM_DATA_SOURCE))
					dataSource = Utility::explode(configValues[PARAM_DATA_SOURCE], ',');
				else
					dataSource = {DATA_SOURCE_FILE};

				GameRecord gd;
				for (string& ds : dataSource) {
					Utility::trim(ds);
					try {
						if (ds == DATA_SOURCE_MAME) {
							gd = parseMame(data[0]);
							if (gd.players == "0")
								continue;
							LogDebug("got " + gd.players + " players data from " DATA_SOURCE_MAME);
							break;
						}
						if (ds == DATA_SOURCE_FILE) {
							gd = parseMameDataFile(data[0]);
							if (gd.players == "0")
								continue;
							LogDebug("got " + gd.players + " players data from " DATA_SOURCE_FILE);
							break;
						}
						if (ds == DATA_SOURCE_CONTROLSINI) {
							gd = parseControlsIni(data[0]);
							if (gd.players == "0")
								continue;
							LogDebug("got " + gd.players + " players data from " DATA_SOURCE_CONTROLSINI);
							break;
						}
					}
					catch (Error& e) {
						LogDebug("Error: " + e.getMessage());
						continue;
					}
				}
				if (gd.players == "0") {
					LogError("No player data detected");
					return EXIT_SUCCESS;
				}
				if (useColors)
					decorateWithColorsIni(data[0], gd);
				if (craftProfile) {
					msg.setType(Message::Types::CraftProfile);
					for (string& s : gd.toString())
						msg.addData(s);
				}
				else {
					msg.addData("P" + gd.players + "_B" + gd.playersData[0].buttons);
					msg.addData(gd.playersData.begin()->second.controllers.front() + gd.players + "_B" + gd.playersData.begin()->second.buttons);
				}

				// Rotate restrictors.
				if (rotate)
					gd.rotate();
				else
					LogDebug("No restrictors found");
			}
			msg.addData(data[1]);
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

GameRecord parseMameDataFile(const string& rom) {

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

	LogDebug("Game data: " + output);

	tinyxml2::XMLDocument xml;
	if (xml.Parse(output.c_str(), output.size()) != tinyxml2::XML_SUCCESS)
		throw Error("Invalid games data file" + output);

	tinyxml2::XMLElement* element = xml.RootElement();
	if (not element)
		throw Error("Missing input node for " + rom);

	return parseMameData(rom, element, true);
}

GameRecord parseMame(const string& rom) {

	string
		output,
		cmd = "mame -lx '" + rom + "'";
	LogDebug("running: " + cmd);

	std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
	if (not pipe)
		throw Error("Failed to gather MAME data");

	std::array<char, 128> buffer;
	while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
		output += buffer.data();

	if (not output.size())
		throw Error("Game " + rom + " not found.");

	tinyxml2::XMLDocument xml;
	if (xml.Parse(output.c_str(), output.size()) != tinyxml2::XML_SUCCESS)
		throw Error("Invalid games data file" + output);

	tinyxml2::XMLElement* element = xml.RootElement()->FirstChildElement(MAME_MACHINE_NODE)->FirstChildElement(MAME_INPUT_NODE);
	if (not element)
		throw Error("Missing input node for " + rom);

	return parseMameData(rom, element, false);
}

GameRecord parseControlsIni(const string& rom) {

	string cmd = "sed -n '/"+ rom +"/,$p' " CONTROLINI_FILE;

	LogDebug("Running: " + cmd);

	std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
	if (not pipe)
		throw Error("Failed to gather MAME data");

	GameRecord tempData;
	std::array<char, 128> buffer;
	bool
		found  = false,
		alter  = false,
		mirror = false;

	vector<string> buttons;
	vector<string> controls;

	while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {

		string tmp = buffer.data();

		if (not found and tmp.find("[" + rom + "]") != string::npos) {
			found = true;
			continue;
		}
		Utility::trim(tmp);

		if (tmp.empty())
			break;

		auto parts = Utility::explode(tmp, '=');
		if (parts.size() != 2)
			continue;

		if (parts[0] == "numPlayers") {
			tempData.players = parts[1];
			continue;
		}

		if (parts[0] == "alternating") {
			alter = parts[1] == "1";
			continue;
		}

		if (parts[0] == "mirrored") {
			mirror = parts[1] == "1";
			continue;
		}

		if (parts[0].find("NumButtons") != string::npos) {
			buttons.push_back(parts[1]);
			continue;
		}

		if (parts[0].find("Controls") != string::npos) {
			controls.push_back(parts[1]);
			continue;
		}
	}
	uint8_t ps = Utility::parseNumber(tempData.players, "Invalid player number");
	if (not ps)
		throw Error("Game " + rom + " not found.");

	for (uint8_t pIx = 0 ; pIx < ps ; ++pIx) {
		string player = to_string(pIx + 1);
		PlayerData& pd = tempData.playersData[player];
		if (mirror) {
			controlIniController2ledspicer(controls[0], pd);
			pd.player  = player;
			pd.buttons = buttons[0];
		}
		else {
			controlIniController2ledspicer(controls[pIx], pd);
			pd.player  = player;
			pd.buttons = buttons[pIx];
		}
	}
	tempData.coins = alter ? "1" : tempData.players;

	return tempData;
}

GameRecord parseMameData(const string& rom, tinyxml2::XMLElement* inputNode, bool compressed) {

	GameRecord tempData;

	umap<string, string> tempAttr = XMLHelper::processNode(inputNode);

	string target = compressed ? PLAYERS : CPLAYERS;
	if (not tempAttr.count(target))
		throw Error("Missing players attribute for " + rom);

	tempData.players = tempAttr[target];

	target = compressed ? COINS : CCOINS;
	if (tempAttr.count(target))
		tempData.coins = tempAttr[target];

	tinyxml2::XMLElement*element = inputNode->FirstChildElement(compressed ? CONTROL : CCONTROL);
	if (not element)
		throw Error("Missing control section " + rom);

	target = compressed ? TYPE : CTYPE;
	string ways = compressed ? WAYS : CWAYS;

	for (; element; element = element->NextSiblingElement()) {

		tempAttr = XMLHelper::processNode(element);
		if (not tempAttr.count(target))
			continue;

		string target2 = compressed ? PLAYER : CPLAYER;
		string player;
		if (tempAttr.count(target2))
			player = tempAttr[target2];
		else
			player = "1";

		PlayerData& pd = tempData.playersData[player];

		pd.player = player;

		if (tempAttr[target] == "stick")
			pd.ways.push_back("analog");
		pd.controllers.push_back(mameController2ledspicer(tempAttr[target]));

		if (tempAttr.count(ways))
			pd.ways.push_back(tempAttr[ways]);

		target2 = compressed ? BUTTONS : CBUTTONS;
		if (tempAttr.count(target2))
			pd.buttons = tempAttr[target2];

		if ((tempAttr[target] == "doublejoy" or tempAttr[target] == "triplejoy") and tempAttr.count(ways + "2")) {
			pd.ways.push_back(tempAttr[ways + "2"]);
			pd.controllers.push_back(JOYSTICK);
		}
		if (tempAttr[target] == "triplejoy" and tempAttr.count(ways + "3")) {
			pd.ways.push_back(tempAttr[ways + "3"]);
			pd.controllers.push_back(JOYSTICK);
		}
	}
	return tempData;
}

void decorateWithColorsIni(const string& rom, GameRecord& gr) {

	string cmd = "sed -n '/"+ rom +"/,$p' " COLORINI_FILE;

	LogDebug("Reading color profile: " + cmd);

	std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
	if (not pipe)
		throw Error("Failed to gather color data");

	std::array<char, 128> buffer;
	bool found  = false;

	while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {

		string tmp = buffer.data();


		if (not found and tmp.find("[" + rom + "]") != string::npos) {
			found = true;
			continue;
		}
		Utility::trim(tmp);

		if (tmp.find("[") != string::npos)
			break;

		auto parts = Utility::explode(tmp, '=');
		if (parts.size() != 2)
			continue;


		auto pair = Utility::explode(parts[0], '_');
			if (pair.size() != 2)
				continue;

		string player = string() + pair[0].back();

		PlayerData& pd = gr.playersData[player];
		if (pd.player.empty())
			pd.player = player;

		/*
		 * Looks like the color file does not provide
		 * information on the controller number, all games are one
		 * controller only, but ledspicer supports multiple.
		 * Some games with different controllers are provided check aafb
		 */
		if (pair[1] != "COIN" and pair[1] != "START" and pair[1].find("BUTTON") == string::npos) {
			uint8_t jIdx = 1;
			for (auto& c : pd.controlColors)
				if (c.first.find(JOYSTICK) != string::npos)
					++jIdx;
			if (pair[1] == "STICK")
				parts[0] = "P" + player + "_" + JOYSTICK;
			parts[0] +=  to_string(jIdx);
			LogDebug("Found color " + parts[1] + " for controller " + parts[0]);
			pd.controlColors.emplace(parts[0], parts[1]);
			continue;
		}
		LogDebug("Found color " + parts[1] + " for button " + parts[0]);
		pd.buttonColors.emplace(parts[0], parts[1]);
		continue;
	}
}

string mameController2ledspicer(const string& controller) {
	if (controller == "mouse")
		return MOUSE;
	if (controller == "lightgun")
		return LIGHTGUN;
	if (controller == "trackball")
		return TRACKBALL;
	if (controller == "dial")
		return DIAL;
	if (controller == "paddle")
		return PADDLE;
	if (controller == "pedal")
		return PEDAL;
	if (controller == "only_buttons")
		return "";
	if (controller == "positional")
		return POSITIONAL;
	return JOYSTICK;
}

void controlIniController2ledspicer(const string& controller, PlayerData& pd) {

	// Order of search is important.
	if (controller.find("vjoy2way") != string::npos) {
		pd.controllers.push_back(JOYSTICK);
		pd.ways.push_back("vertical2");
		return;
	}

	if (controller.find("vdoublejoy2way") != string::npos) {
		pd.controllers.push_back(JOYSTICK);
		pd.controllers.push_back(JOYSTICK);
		pd.ways.push_back("vertical2");
		pd.ways.push_back("vertical2");
		return;
	}

	if (controller.find("joy2way") != string::npos) {
		pd.controllers.push_back(JOYSTICK);
		pd.ways.push_back("2");
		return;
	}

	if (controller.find("doublejoy4way") != string::npos) {
		pd.controllers.push_back(JOYSTICK);
		pd.controllers.push_back(JOYSTICK);
		pd.ways.push_back("4");
		pd.ways.push_back("4");
		return;
	}

	if (controller.find("joy4way") != string::npos) {
		pd.controllers.push_back(JOYSTICK);
		if (controller.find("Diagonal") != string::npos)
			pd.ways.push_back("4x");
		else
			pd.ways.push_back("4");
		return;
	}

	if (controller.find("doublejoy8way") != string::npos) {
		pd.controllers.push_back(JOYSTICK);
		pd.controllers.push_back(JOYSTICK);
		pd.ways    = {"8", "8"};
		pd.ways.push_back("8");
		pd.ways.push_back("8");
		return;
	}

	if (controller.find("joy8way") != string::npos) {
		pd.controllers.push_back(JOYSTICK);
		pd.ways    = {"8"};
		pd.ways.push_back("8");
		return;
	}

	if (controller.find("dial") != string::npos) {
		pd.controllers.push_back(DIAL);
		return;
	}

	if (controller.find("trackball") != string::npos) {
		pd.controllers.push_back(TRACKBALL);
		return;
	}

	if (controller.find("lightgun") != string::npos) {
		pd.controllers.push_back(LIGHTGUN);
		return;
	}
}

vector<string> GameRecord::toString() {

	vector<string> result;
	string pd;
	for (auto& playerData : playersData)
		pd += playerData.second.toString();
	result.push_back(pd.substr(0, pd.size()-1));
	pd.clear();

	for (uint8_t c = 1; c <= Utility::parseNumber(players, ""); ++c)
		pd += "PLAYER" + to_string(c) + ",";

	result.push_back(pd + (coins.empty() ? "" : coins + "_COINS,") + players + "_PLAYERS");
	return result;
}

void GameRecord::rotate() {
	string command = "rotator ";
	for (auto& pd : playersData)
		command += pd.second.rotate();
	if (command == "rotator ") {
		LEDSpicer::Log::debug("No rotating information found");
		return;
	}
	LEDSpicer::Log::debug("Running: " + command);
	if (system(command.c_str()) != EXIT_SUCCESS)
		LogWarning("Failed to execute " + command);
}

string PlayerData::toString() {

	string p;

	for (uint8_t c = 0; c < controllers.size(); ++c) {
		uint8_t cIx = 1;
		string con = "P" + player + "_" + controllers[c];
		while (true) {
			if (p.find(con + to_string(cIx)) != string::npos)
				++cIx;
			else
				break;

		}
		con += to_string(cIx);
		p += con;
		if (controlColors.count(con))
			p += ":" + controlColors[con];
		p += ",";
	}

	uint8_t bTo = Utility::parseNumber(buttons, "");
	for (uint8_t c = 0; c < bTo; ++c) {
		string but = "P" + player + "_BUTTON" + to_string(c + 1);
		p += but;
		if (buttonColors.count(but))
			p += ":" + buttonColors[but];
		p += ",";
	}

	// Extra colors detected:
	for (auto& c : controlColors)
		if (p.find(c.first) == string::npos)
			p += c.first + ":" + c.second + ",";

	for (auto& c : buttonColors)
		if (p.find(c.first) == string::npos)
			p += c.first + ":" + c.second + ",";

	return p;
}

string PlayerData::rotate() {
	string command;
	for (uint8_t c = 0; c < ways.size(); ++c)
		command += player + " " + to_string(c + 1) + " '" + ways[c] + "' ";
	return command;
}
