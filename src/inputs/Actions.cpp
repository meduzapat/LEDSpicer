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

Actions::Actions(umap<string, string>& parameters, umap<string, Items*>& inputMaps) :
	Reader(parameters, inputMaps),
	Speed(parameters.count("speed") ? parameters["speed"] : ""),
	frames(static_cast<uint8_t>(speed) * 3),
	doBlink(parameters.count("blink") ? (parameters["blink"] == "true") : true)
{
	string linkedTriggers = parameters.count("linkedTriggers") ? parameters["linkedTriggers"] : "";
	// process list
	if (linkedTriggers.empty())
		return;

	uint groupIdx = 0;
	// This is a list of linked maps separated by RECORD_SEPARATOR
	for (auto& l : Utility::explode(linkedTriggers, RECORD_SEPARATOR)) {
		// This is a list of two or more triggers, separated by INPUT_MAP_SEPARATOR.
		auto group = Utility::explode(l, FIELD_SEPARATOR);
		if (group.size() < 2) {
			LogWarning("Ignoring group with less than two items.");
			continue;
		}
		// Extract group info.
		vector<Record> tempRecord;
		uint mapIdx = 0;
		bool firstItem = true;
		for (auto& link : group) {
			Utility::trim(link);
			uint16_t map = Utility::parseNumber(link, "Unable to parse number");
			tempRecord.emplace_back(std::move(Record(map, firstItem, itemsMap[link], nullptr)));
			if (firstItem) {
				firstItems.push_back(map);
				firstItem = false;
			}
			// Update lookup table.
			groupMapLookup.emplace(map, LookupMap{groupIdx, mapIdx++});
		}
		// Link last element to 1st element.
		tempRecord.back().next = &tempRecord[0];
		// Link rest of the table.
		for (uint c = 0; c < tempRecord.size() - 1; ++c)
			tempRecord[c].next = &tempRecord[c + 1];

		groupsMaps.emplace_back(std::move(tempRecord));
		++groupIdx;
	}
}

void Actions::process() {

	readAll();

	blink();

	if (not events.size())
		return;

	for (auto& event : events) {

		if (not event.value)
			continue;

		string eventCodeStr = to_string(event.code);
		if (not itemsMap.count(eventCodeStr))
			continue;

		if (controlledItems->count(eventCodeStr))
			controlledItems->erase(eventCodeStr);

		// Non Grouped elements.
		if (not groupMapLookup.count(event.code)) {
			if (blinkingItems.count(event.code)) {
				LogDebug("key: " + eventCodeStr + " for " + itemsMap[eventCodeStr]->getName() + " stop blinking");
				blinkingItems.erase(event.code);
			}
			else {
				LogDebug("key: " + eventCodeStr + " for " + itemsMap[eventCodeStr]->getName() + " start blinking");
				blinkingItems.emplace(event.code, itemsMap[eventCodeStr]);
			}
			continue;
		}

		// grouped elements.
		Record& groupMap = groupsMaps[groupMapLookup[event.code].groupIdx][groupMapLookup[event.code].mapIdx];
		if (groupMap.active) {
			groupMap.active = false;
			blinkingItems.erase(event.code);
			blinkingItems.emplace(groupMap.next->map, groupMap.next->item);
			groupMap.next->active = true;
			LogDebug("key: " + eventCodeStr + " for " + groupMap.item->getName() + " switches to: " + groupMap.next->item->getName());
		}
	}
}

void Actions::activate() {
	for (uint16_t map : firstItems) {
		Record& groupMap = groupsMaps[groupMapLookup[map].groupIdx][groupMapLookup[map].mapIdx];
		blinkingItems.emplace(map, groupMap.item);
		groupMap.active = true;
	}
	Reader::activate();
}

void Actions::deactivate() {
	for (auto& g : groupsMaps)
		for (auto& i : g)
			i.active = false;
	blinkingItems.clear();
	Reader::deactivate();
}

void Actions::drawConfig() {
	cout << SEPARATOR << endl << "Type: Actions" << endl;
	Reader::drawConfig();
	if (groupsMaps.size()) {
		cout << "Linked Items: " << endl;
		for (auto& g : groupsMaps) {
			for (auto& i : g)
				cout << i.item->getName() << " → ";
			cout << "«" << endl;
		}
	}
	Speed::drawConfig();
}

void Actions::blink() {
	if (not doBlink) {
		for (auto& e : blinkingItems)
			if (not controlledItems->count(std::to_string(e.first)))
				controlledItems->emplace(std::to_string(e.first), e.second);
		return;
	}
	if (frames == cframe) {
		cframe = 0;
		for (auto& e : blinkingItems) {
			if (on)
				controlledItems->erase(std::to_string(e.first));
			else
				controlledItems->emplace(std::to_string(e.first), e.second);
		}
		on = not on;
	}
	else {
		++cframe;
	}
}
