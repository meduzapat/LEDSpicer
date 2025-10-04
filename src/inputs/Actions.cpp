/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Actions.cpp
 * @since     May 23, 2019
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

#include "Actions.hpp"

using namespace LEDSpicer::Inputs;

inputFactory(Actions)

Actions::Actions(StringUMap& parameters, ItemPtrUMap& inputMaps) :
	Reader(parameters, inputMaps),
	Speed(parameters.exists("speed") ? parameters["speed"] : "Normal"),
	frames(static_cast<uint8_t>(speed) * 3),
	doBlink(parameters.exists("blink") ? (parameters["blink"] == "True") : true)
{
	string linkedTriggers = parameters.exists("linkedTriggers") ? parameters["linkedTriggers"] : "";

	// Process list.
	if (linkedTriggers.empty())
		return;

	uint groupIdx = 0;
	// This is a list of linked maps (position) separated by ID_GROUP_SEPARATOR ex: 1,2,3|4,5,6|etc..
	for (auto& linkGroup : Utility::explode(linkedTriggers, ID_GROUP_SEPARATOR)) {
		// This is a list of two or more positions, separated by ID_SEPARATOR ex: 1,2,3.
		auto groupMapIDs = Utility::explode(linkGroup, ID_SEPARATOR);
		if (groupMapIDs.size() < 2) {
			LogWarning("Ignoring group with less than two items.");
			continue;
		}
		// Extract group info.
		vector<Record> tempRecord;
		uint mapIdx = 0;
		for (auto& mapId : groupMapIDs) {
			Utility::trim(mapId);
			// Convert mapId to trigger.
			uint16_t mapCode = Utility::parseNumber(mapId, "Unable to parse map ID, is not a number");
			string trigger;
			// Find trigger by position from items map.
			for (const auto& pair : itemsUMap) {
				if (pair.second->pos == mapCode) {
					trigger = pair.first;
					break;
				}
			}
			if (trigger.empty()) {
				LogWarning("Ignoring invalid map ID " + mapId);
				continue;
			}
			tempRecord.emplace_back(std::move(Record{trigger, mapIdx == 0, itemsUMap[trigger], nullptr}));
			// Update lookup table.
			groupMapLookup.emplace(trigger, LookupMap{groupIdx, mapIdx++});
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

		if (not event.value) continue;
		if (not itemsUMap.exists(event.trigger)) continue;

		// If the trigger already exist remove it so the next can be set
		removeControlledItemByTrigger(event.trigger);

		// Non Grouped elements.
		if (not groupMapLookup.exists(event.trigger)) {
			if (blinkingItems.exists(event.trigger)) {
				LogDebug("key: " + event.trigger + " for " + itemsUMap[event.trigger]->getName() + " stop blinking");
				blinkingItems.erase(event.trigger);
			}
			else {
				LogDebug("key: " + event.trigger + " for " + itemsUMap[event.trigger]->getName() + " start blinking");
				blinkingItems.emplace(event.trigger, itemsUMap[event.trigger]);
			}
			continue;
		}

		auto& lookupMap(groupMapLookup[event.trigger]);
		// grouped elements.
		Record& groupMap = groupsMaps[lookupMap.groupIdx][lookupMap.mapIdx];

		if (not groupMap.active)
			return;

		// Move to the next element.
		groupMap.active = false;
		blinkingItems.erase(event.trigger);
		blinkingItems.emplace(groupMap.next->map, groupMap.next->item);
		groupMap.next->active = true;
		LogDebug("key: " + event.trigger + " for " + groupMap.item->getName() + " switches to: " + groupMap.next->item->getName());
	}
}

void Actions::activate() {
	for (auto& group : groupsMaps) {
		Record& groupMap = group.front();
		blinkingItems.emplace(groupMap.map, groupMap.item);
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

void Actions::drawConfig() const {
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
	cout << "Blink: " << (doBlink ? "True" : "False") << endl;
	Speed::drawConfig();
}

void Actions::blink() {
	if (not doBlink) {
		for (auto& e : blinkingItems)
			if (not controlledItems.exists(e.first))
				controlledItems.emplace(e.first, e.second);
		return;
	}
	if (frames == cframe) {
		cframe = 0;
		for (auto& e : blinkingItems) {
			if (on)
				controlledItems.erase(e.first);
			else
				controlledItems.emplace(e.first, e.second);
		}
		on = not on;
	}
	else {
		++cframe;
	}
}
