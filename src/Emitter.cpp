/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Emiter.cpp
 * @since     Jul 8, 2018
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2018 - 2019 Patricio A. Rossi (MeduZa)
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
		rawMessage;

	Message msg;

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
				" binary rom emulator  Attempts to load a profile based on the emulator and the ROM.\n" <<
				Message::type2str(Message::Types::FinishLastProfile) <<
				"                           Terminates the current profile (doens't affect the default).\n" <<
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
				"                              Removes all group's background color.\n\n"
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
				PACKAGE_STRING " Copyright © 2018 - 2019 - Patricio A. Rossi (MeduZa)\n\n"
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

		Utility::checkAttributes({"port"}, configValues, "root");

		// Set log level.
		if (configValues.count("logLevel"))
			Log::setLogLevel(Log::str2level(configValues["logLevel"]));

		// Check request.
		if (msg.getType() == Message::Types::LoadProfileByEmulator) {
			if (msg.getData().size() < 3) {
				LogError("Error: Invalid request");
				return EXIT_FAILURE;
			}
			string
				binary = msg.getData()[0],
				load   = msg.getData()[1],
				name   = msg.getData()[2];
			Message msgTemp;

			msgTemp.setType(Message::Types::LoadProfile);
			msgTemp.addData(string(name).append("/").append(load));
			// mame
			if (name == "mame") {
				for (string& p : parseMame(load))
					if (not p.empty())
						msgTemp.addData(p);
			}
			msgTemp.addData(name);
			msg = msgTemp;
		}

		// Open connection and send message.
		Socks sock(LOCALHOST, configValues["port"]);
		bool r = sock.send(msg.toString());
		LogDebug("Message " + string(r ? "sent successfully: " : "failed to send: ") + msg.toString());
	}
	catch(Error& e) {
		LogError("Error: " + e.getMessage());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

vector<string> parseMame(const string& rom) {

	vector<string> result(2);
	string
		output,
		cmd = "grep -F '\"" + rom + "\"' " + CONTROLLERS_FILE;
	LogDebug("running: " + cmd);

	std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
	if (not pipe) {
		LogError("Failed to read games data file");
		return result;
	}

	std::array<char, 128> buffer;
	while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
		output += buffer.data();

	tinyxml2::XMLDocument xml;
	if (xml.Parse(output.c_str(), output.size()) != tinyxml2::XML_SUCCESS) {
		LogError("Invalid games data file" + output);
		return result;
	}

	LogDebug("Game data: " + output);
/* Possible types
<control type="joy" player="2" buttons="1" ways="2|4|8"/>
<control type="stick" player="1" buttons="4" minimum="0" maximum="255" sensitivity="30" keydelta="30" reverse="yes"/>
<control type="keypad" player="1" buttons="24"/>
<control type="paddle" buttons="2" minimum="0" maximum="63" sensitivity="100" keydelta="1"/>
<control type="pedal" buttons="3" minimum="0" maximum="31" sensitivity="100" keydelta="1"/>
<control type="dial" buttons="4" minimum="0" maximum="255" sensitivity="25" keydelta="20"/>
<control type="only_buttons" buttons="7"/>
<control type="mouse" player="1" minimum="0" maximum="255" sensitivity="100" keydelta="5"/>
<control type="lightgun" player="1" buttons="1" minimum="0" maximum="255" sensitivity="70" keydelta="10"/>
<control type="trackball" player="1" buttons="1" minimum="0" maximum="255" sensitivity="100" keydelta="10" reverse="yes"/>
<control type="doublejoy" buttons="1" ways="8" ways2="8"/>

*/
	umap<string, string> tempAttr;
	uint8_t
		players = 0,
		buttons = 0;
	tinyxml2::XMLElement* element = xml.RootElement();
	if (not element) {
		LogWarning("Missing input node for " + rom);
		return result;
	}
	tempAttr = XMLHelper::processNode(element);

	if (not tempAttr.count(PLAYERS)) {
		LogWarning("Missing players attribute for " + rom);
		return result;
	}
	result[1] = "P" + tempAttr[PLAYERS] + "_B";

	element = element->FirstChildElement(CONTROL);
//	for (; element; element = element->NextSiblingElement("control")) {
//	}
	tempAttr = XMLHelper::processNode(element);
	result[0] = tempAttr[TYPE] + tempAttr[PLAYERS] + "_B";
	if (tempAttr.count(BUTTONS)) {
		result[0].append(tempAttr[BUTTONS]);
		result[1].append(tempAttr[BUTTONS]);
	}
	return result;
}


