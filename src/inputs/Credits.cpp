/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Credits.cpp
 * @since     Apr 15, 2024
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2018 - 2026 Patricio A. Rossi (MeduZa)
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

#include "Credits.hpp"

using namespace LEDSpicer::Inputs;

inputFactory(Credits)

Credits::Credits(StringUMap& parameters, ItemPtrUMap& inputMaps) :
	Reader(parameters, inputMaps),
	Speed(parameters.exists("speed") ? parameters["speed"] : "Normal"),
	frames(static_cast<uint8_t>(speed) * 3),
	coinsPerCredit(Utility::parseNumber(parameters.exists("coinsPerCredit") ? parameters["coinsPerCredit"] : DEFAULT_COINS, "Invalid numeric value ")),
	mode((parameters.exists("mode") ? parameters["mode"] : DEFAULT_MODE) == DEFAULT_MODE ? Modes::Multi : Modes::Single),
	once(parameters.exists("once") ? parameters["once"] == "True" : false),
	alwaysOn(parameters.exists("alwaysOn") ? parameters["alwaysOn"] == "True": false)
{

	string linkedTriggers = parameters.exists("linkedTriggers") ? parameters["linkedTriggers"] : "";

	// Process list.
	if (linkedTriggers.empty())
		throw Error("Credits needs at least one group of 2 or more elements");

	uint groupIdx = 0;
	// This is a list of linked maps (position) separated by ID_GROUP_SEPARATOR ex: 1,2,3|4,5,6|etc..
	for (auto& linkGroup : Utility::explode(linkedTriggers, ID_GROUP_SEPARATOR)) {
		// This is a list of two or more positions, separated by ID_SEPARATOR ex: 1,2,3.
		auto groupMapIDs = Utility::explode(linkGroup, ID_SEPARATOR);
		if (groupMapIDs.size() < 2) {
			LogWarning("Ignoring group with less than two items");
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
			tempRecord.emplace_back(std::move(Record{trigger, mapIdx == 0, itemsUMap[trigger]}));
			// Update lookup table.
			groupMapLookup.emplace(trigger, LookupMap{groupIdx, mapIdx++});
		}

		groupsMaps.emplace_back(std::move(tempRecord));
		++groupIdx;
	}

	coinCount.assign(groupsMaps.size(), 0);
}

void Credits::process() {

	readAll();

	blink();

	if (not events.size()) return;

	for (auto &event : events) {

		if (not event.value) continue;

		if (not itemsUMap.exists(event.trigger)) continue;

		// Lookup Trigger.
		auto& lookupMap(groupMapLookup[event.trigger]);
		// Pick record data.
		Record& groupMap = groupsMaps[lookupMap.groupIdx][lookupMap.mapIdx];
		// Not active, done.
		if (not groupMap.active) continue;

		bool isCoin = lookupMap.mapIdx == 0;

		// Coin inserted
		if (isCoin) {
			LogDebug("Coin inserted by " + groupMap.item->getName());
			++coinCount[lookupMap.groupIdx];
			// Check number of coins.
			if (coinCount[lookupMap.groupIdx] < coinsPerCredit) continue;
			// Credit issued, reset coin count.
			LogDebug("Credit issued by " + groupMap.item->getName());
			coinCount[lookupMap.groupIdx] = 0;
			// Find next Start.
			Record* record = nullptr;
			size_t total   = groupsMaps[lookupMap.groupIdx].size();
			size_t active  = 0;
			// Find first OFF start and check if all Starts are on.
			for (size_t c = 1; c < total; ++c) {
				if (not groupsMaps[lookupMap.groupIdx][c].active) {
					if (not record) {
						record = &groupsMaps[lookupMap.groupIdx][c];
						LogDebug("Start OFF detected " + record->item->getName());
						++active;
					}
				}
				else {
					++active;
				}
			}

			bool allOn = active == total - 1;

			if (groupMap.active and not alwaysOn and ((allOn and mode == Modes::Multi) or mode == Modes::Single)) {
				// Stop Coin Blinking.
				LogDebug("Stop Coin for " + groupMap.item->getName());
				removeControlledItemByTrigger(event.trigger);
				blinkingItems.erase(event.trigger);
				if (mode == Modes::Multi or allOn)
					groupMap.active = false;
			}

			if (not record) continue;

			// Set next Start on
			record->active = true;
			blinkingItems.emplace(record->map, record->item);
			LogDebug("Start: " + record->item->getName() + " Active");
		}
		else {
			removeControlledItemByTrigger(event.trigger);
			groupMap.active = false;
			blinkingItems.erase(event.trigger);

			if (not once and not alwaysOn) {
				// Set start back on.
				Record& coin = groupsMaps[lookupMap.groupIdx][0];
				coin.active = true;
				if (not blinkingItems.exists(coin.map)) {
					blinkingItems.emplace(coin.map, coin.item);
					LogDebug("Credit: " + coin.item->getName() + " Active");
				}
			}
		}
	}
}

void Credits::activate() {
	for (auto& group : groupsMaps) {
		for (size_t c = 0; c < group.size(); ++c) {
			if (c == 0) {
				blinkingItems.emplace(group[c].map, group[c].item);
				group[c].active = true;
			}
			else {
				group[c].active = false;
			}
		}
	}
	Reader::activate();
}

void Credits::deactivate() {
	for (auto& g : groupsMaps)
		for (auto& i : g)
			i.active = false;
	blinkingItems.clear();
	Reader::deactivate();
}

void Credits::drawConfig() const {
	cout << SEPARATOR << endl << "Type: Credits" << endl;
	Reader::drawConfig();
	cout << "Linked Items: " << endl;
	for (auto& g : groupsMaps) {
		for (size_t i = 0; i < g.size(); ++i) {
			if (i)
				cout << " Start: " << g[i].item->getName();
			else
				cout << "Coin: " << g[i].item->getName() << " For:";
		}
		cout << endl;
	}
	Speed::drawConfig();
	cout <<
		"Mode: " << (mode == Modes::Single ? "Single" : "Multi") << endl <<
		"Coins Per Credit: " << coinsPerCredit                   << endl <<
		"Once: "             << (once ? "True" : "False")        << endl <<
		"Always On: "        << (alwaysOn ? "True" : "False")    << endl;
}

void Credits::blink() {
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
