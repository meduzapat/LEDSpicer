/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      InputSeeker.cpp
 * @since     Jun 22, 2019
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

#include "InputSeeker.hpp"

using namespace LEDSpicer;

termios termInfo;
void resetInputMode() {
	tcsetattr(STDIN_FILENO, TCSANOW, &termInfo);
}

int main(int argc, char **argv) {

	cout <<
		"Input Seeker v" PACKAGE_VERSION << endl <<
		"Part of LEDSpicer project" << endl <<
		"This program will open all the system inputs and listen for events to easy the task of setting input plugins" << endl << endl <<
		PACKAGE_STRING " Copyright © 2018 - 2020 - Patricio A. Rossi (MeduZa)\n\n"
		"For more information visit <" PACKAGE_URL ">\n\n"
		"To report errors or bugs visit <" PACKAGE_BUGREPORT ">\n"
		PACKAGE_NAME " is free software under the GPL 3 license\n\n"
		"See the GNU General Public License for more details <http://www.gnu.org/licenses/>." << endl << endl <<
		"press q to exit" << endl << endl;

	DIR *dir;
	struct dirent *ent;
	string name;
	size_t pos;
	if (not (dir = opendir(DEV_INPUT))) {
		cerr << "Unable to read " DEV_INPUT << endl;
		return EXIT_FAILURE;
	}
	while ((ent = readdir(dir)) != nullptr) {
		name = ent->d_name;
		if (name.find("event") ==string::npos)
			continue;
		cout << "Opening device " << name << endl;
		int res = open((DEV_INPUT + name).c_str(), O_RDONLY | O_NONBLOCK);
		if (res < 0) {
			cerr << "Unable to open " DEV_INPUT << name << " Ignored" << endl;
			continue;
		}
		listenEvents.emplace(name, res);
	}
	closedir(dir);


	if (!isatty(STDIN_FILENO)) {
		cerr << "Not a terminal" << endl;
		return EXIT_FAILURE;
	}

	// Save Terminal settings.
	tcgetattr(STDIN_FILENO, &termInfo);
	atexit(resetInputMode);

	// Clear Terminal ICANON and ECHO.
	termios tattr;
	tcgetattr(STDIN_FILENO, &tattr);
	tattr.c_lflag &= ~(ICANON | ECHO);
	tattr.c_cc[VMIN] = 1;
	tattr.c_cc[VTIME] = 0;
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &tattr);

	bool running = true;
	while (running)
		for (auto& l : listenEvents) {
			input_event event;
			char buffer[sizeof(event)];
			while (running) {
				ssize_t r = read(l.second, reinterpret_cast<void*>(&event), sizeof(event));
				if (r < 1)
					break;

				// If q is press stop.
				if (not event.value and event.type == EV_KEY and event.code == 16)
					running = false;

				if (not event.type)
					continue;

				cout << "Listen Event: " << l.first << " code: " << event.code << " Type: ";

				switch (event.type) {
				case EV_KEY:
					cout << "KEY" << (event.value ? "(On)" : "(Off)");
					break;
				case EV_REL:
				case EV_ABS:
					cout << "MOVE";
					// not needed
/*					switch (event.code) {
					case REL_DIAL:
					case REL_RX:
					case REL_X:
						cout << "("<< (event.value < 0 ? "L" : (event.value > 0 ? "R" : "0")) << ")";
						break;
					case REL_HWHEEL:
					case REL_WHEEL:
					case REL_Y:
					case REL_RY:
						cout << "("<< (event.value < 0 ? "U" : (event.value > 0 ? "D" : "0")) << ")";
						break;
					case REL_RZ:
					case REL_Z:
						cout << "("<< (event.value < 0 ? "B" : (event.value > 0 ? "F" : "0")) << ")";
						break;
					}*/
					break;
				default:
					cout << "OTHER (" << event.type << ")";
					break;
				}
				cout << endl;
			}
	}

	for (auto& l : listenEvents) {
		cout << "Closing device " DEV_INPUT << l.first << endl;
		close(l.second);
		l.second = -1;
	}

	// Flush stdin
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &tattr);
	return EXIT_SUCCESS;
}
