/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Actions.cpp
 * @since     May 23, 2019
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

#include "Actions.hpp"

using namespace LEDSpicer::Inputs;

void Actions::process() {

	readAll();

	blink();

	if (not events.size())
		return;

	for (auto& event : events) {

		if (not event.value)
			continue;

		string codeName = std::to_string(event.code);
		if (elementMap.count(codeName)) {
			string fullname = codeName + elementMap[codeName].element->getName();
			if (linkedElements.count(codeName)) {
				string linkedCode = linkedElements[codeName];
				string linkedName = linkedCode + elementMap[linkedCode].element->getName();
				// switch
				if (blinkingElements.count(linkedName)) {
					blinkingElements.erase(linkedName);
					controlledElements->erase(linkedName);
					blinkingElements.emplace(fullname, &elementMap[codeName]);
					LogDebug("key: " + codeName + " switches: " + linkedName + " to: " + codeName);
				}
				continue;
			}
			if (blinkingElements.count(fullname)) {
				// deactivate
				blinkingElements.erase(fullname);
				controlledElements->erase(fullname);
				LogDebug("key: " + codeName + " go off: " + fullname);
			}
			else if (not linkedElements.count(codeName)) {
				// activated
				blinkingElements.emplace(fullname, &elementMap[codeName]);
				LogDebug("key: " + codeName + " go on: " + fullname);
			}
		}

		if (groupMap.count(codeName)) {
			string fullname = codeName + groupMap[codeName].group->getName();
			if (linkedGroup.count(codeName)) {
				string linkedCode = linkedGroup[codeName];
				string linkedName = linkedCode + groupMap[linkedCode].group->getName();
				// switch
				if (blinkingGroups.count(linkedName)) {
					blinkingGroups.erase(linkedName);
					controlledGroups->erase(linkedName);
					blinkingGroups.emplace(fullname, &groupMap[codeName]);
					LogDebug("key: " + codeName + " switches: " + linkedName + " to: " + codeName);
				}
				continue;
			}
			if (blinkingGroups.count(fullname)) {
				// deactivate
				blinkingGroups.erase(fullname);
				controlledGroups->erase(fullname);
				LogDebug("key: " + codeName + " go off: " + fullname);
			}
			else if (not linkedGroup.count(codeName)) {
				// activated
				blinkingGroups.emplace(fullname, &groupMap[codeName]);
				LogDebug("key: " + codeName + " go on: " + fullname);
			}
		}
	}
}

void Actions::activate() {
	// process list
	if (not linkedElementsRaw.empty()) {
		for (auto& l : Utility::explode(linkedElementsRaw, '|')) {
			vector<string> es = Utility::explode(l, ',');
			if (es.size() != 2)
				throw Error("you can only link two elements");
			Utility::trim(es[0]);
			Utility::trim(es[1]);
			preOnElements.push_back(findElementMapByName(es[0]));
			linkedElements.emplace(findElementMapByName(es[0]), findElementMapByName(es[1]));
			linkedElements.emplace(findElementMapByName(es[1]), findElementMapByName(es[0]));
		}
		linkedElementsRaw.clear();
	}

	controlledElements->clear();
	controlledGroups->clear();
	for (string& e : preOnElements)
		blinkingElements.emplace(e + elementMap[e].element->getName(), &elementMap[e]);
	for (string& e : preOnGroups)
		blinkingGroups.emplace(e + groupMap[e].group->getName(), &groupMap[e]);
	Reader::activate();
}

void Actions::deactivate() {
	blinkingElements.clear();
	blinkingGroups.clear();
	Reader::deactivate();
}

void Actions::drawConfig() {
	cout << "Linked Elements: ";
	for (auto&le : linkedElementsRaw)
		cout << le << " ";
	cout << endl;
	Reader::drawConfig();
}

void Actions::blink() {
	if (frames == cframe) {
		cframe = 0;
		for (auto& e : blinkingElements) {
			if (on)
				controlledElements->erase(e.first);
			else
				controlledElements->emplace(e.first, e.second);
		}
		for (auto& e : blinkingGroups) {
			if (on)
				controlledGroups->erase(e.first);
			else
				controlledGroups->emplace(e.first, e.second);
		}
		on = not on;
	}
	else {
		++cframe;
	}
}

string Actions::findElementMapByName(string& name) {
	for (auto& eMap : elementMap) {
		if (eMap.second.element->getName() == name)
			return eMap.first;
	}
	throw Error("Unable to find element named " + name);
}
