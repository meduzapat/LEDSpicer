/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Emiter.cpp
 * @since     Jul 8, 2018
 * @author    Patricio A. Rossi (MeduZa)
 * @copyright Copyright © 2018 Patricio A. Rossi (MeduZa)
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
				Message::type2str(Message::Types::LoadProofileByEmulator) <<
				" emulator rom         Attempts to load a profile based on the emulator and the ROM.\n" <<
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
				"                              Changes all group's background color.\n\n"
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
				PACKAGE_STRING " Copyright © 2018 - Patricio A. Rossi (MeduZa)\n\n"
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
		umap<string, string> configValues;
		const tinyxml2::XMLAttribute* pAttrib = config.getRoot()->FirstAttribute();
		while (pAttrib) {
			string value = pAttrib->Value();
			configValues.emplace(pAttrib->Name(), value);
			pAttrib = pAttrib->Next();
		}
		Utility::checkAttributes({"port"}, configValues, "root");

		// Set log level.
		if (configValues.count("logLevel"))
			Log::setLogLevel(Log::str2level(configValues["logLevel"]));

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
