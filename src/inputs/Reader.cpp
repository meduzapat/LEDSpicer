/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Reader.cpp
 * @since     May 23, 2019
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

#include "Reader.hpp"

using namespace LEDSpicer::Inputs;

vector<input_event> Reader::events;
umap<string, int> Reader::listenEvents;
Input* Reader::readController = nullptr;

Reader::Reader(umap<string, string>& parameters, umap<string, Items*>& inputMaps) : Input(parameters, inputMaps) {
	if (parameters.count("listenEvents"))
		for (auto& e : Utility::explode(parameters["listenEvents"], ID_GROUP_SEPARATOR)) {
			Utility::trim(e);
			if (not listenEvents.count(e))
				listenEvents.emplace(e, -1);
		}
}

void Reader::activate() {
	readController = nullptr;
	for (auto& l : listenEvents) {
		// Ignore already connected elements.
		if (l.second >= 0)
			continue;
		LogInfo("Opening device " + l.first);
		l.second = open((DEV_INPUT + l.first).c_str(), O_RDONLY | O_NONBLOCK);
		if (l.second < 0) {
			LogWarning("Unable to open " DEV_INPUT + l.first);
		}
	}
}

void Reader::deactivate() {
	for (auto& l : listenEvents) {
		if (l.second < 0)
			continue;
		LogInfo("Closing device " DEV_INPUT + l.first);
		close(l.second);
		l.second = -1;
	}
}

void Reader::drawConfig() {
	cout << "Listening events: " << endl;
	for (auto&le : listenEvents)
		cout << "  " << le.first << endl;
	cout << endl;
	Input::drawConfig();
}

void Reader::readAll() {

	if (not readController)
		readController = this;

	if (readController != this)
		return;

	events.clear();
	for (auto& l : listenEvents) {
		if (l.second < 0)
			continue;

		input_event event;
		char buffer[sizeof(event)];
		while (true) {
			ssize_t r = read(l.second, reinterpret_cast<void*>(&event), sizeof(event));
			if (r < 1)
				break;
			if (event.type != EV_KEY) // and event.type != EV_REL))
				continue;
			LogDebug(l.first + " - Type: " + (event.type == 1 ? "Key" : "Other") + " code: " + std::to_string(event.code) + string(event.value ? " ON" : " OFF"));
			events.push_back(event);
		}
	}
}
