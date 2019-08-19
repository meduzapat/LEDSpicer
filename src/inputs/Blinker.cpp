/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Blinker.cpp
 * @since     Aug 17, 2019
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2019 Patricio A. Rossi (MeduZa)
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

#include "Blinker.hpp"

using namespace LEDSpicer::Inputs;

void Blinker::process() {

	readAll();

	for (auto& e : elementsToClean)
		blinkingElements.erase(e);
	for (auto& g : groupsToClean)
		blinkingGroups.erase(g);

	elementsToClean.clear();
	groupsToClean.clear();

	blink();

	if (not events.size())
		return;

	for (auto& event : events) {

		if (not event.value)
			continue;

		string codeName = std::to_string(event.code);
		if (elementMap.count(codeName)) {
			string fullname = codeName + elementMap[codeName].element->getName();
			LogDebug("key: " + codeName + " adds: " + to_string(times) + " times to element: " + fullname);
			// switch
			if (blinkingElements.count(fullname)) {
				blinkingElements[fullname].times = 0;
			}
			else {
				blinkingElements.emplace(fullname, timesElement{&elementMap[codeName], 0});
			}
		}

		if (groupMap.count(codeName)) {
			string fullname = codeName + groupMap[codeName].group->getName();
			LogDebug("key: " + codeName + " adds: " + to_string(times) + " times to group: " + fullname);
			// switch
			if (blinkingGroups.count(fullname)) {
				blinkingGroups[fullname].times = 0;
			}
			else {
				blinkingGroups.emplace(fullname, timesGroup{&groupMap[codeName], 0});
			}
		}
	}
}

void Blinker::activate() {
	controlledElements->clear();
	controlledGroups->clear();
	Reader::activate();
}

void Blinker::deactivate() {
	blinkingElements.clear();
	blinkingGroups.clear();
	Reader::deactivate();
}

void Blinker::drawConfig() {
	cout << "Stop After " << times << " times" << endl;
	Reader::drawConfig();
}

void Blinker::blink() {
	if (frames == cframe) {
		cframe = 0;
		for (auto& e : blinkingElements) {
			// deactivate after time passed.
			if (e.second.times == times) {
				controlledElements->erase(e.first);
				elementsToClean.push_back(e.first);
				continue;
			}
			if (on) {
				controlledElements->erase(e.first);
			}
			else {
				controlledElements->emplace(e.first, e.second.element);
				++e.second.times;
			}
		}
		for (auto& e : blinkingGroups) {
			// deactivate after time passed.
			if (e.second.times == times) {
				controlledGroups->erase(e.first);
				groupsToClean.push_back(e.first);
				continue;
			}
			if (on) {
				controlledGroups->erase(e.first);
			}
			else {
				controlledGroups->emplace(e.first, e.second.group);
				++e.second.times;
			}
		}
		on = not on;
	}
	else {
		++cframe;
	}
}
