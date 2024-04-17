/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Credits.cpp
 * @since     Apr 15, 2024
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

#include "Credits.hpp"

using namespace LEDSpicer::Inputs;

Credits::Credits(umap<string, string>& parameters, umap<string, Items*>& inputMaps) :
	Reader(parameters, inputMaps),
	Speed(parameters.count("speed") ? parameters["speed"] : ""),
	frames(static_cast<uint8_t>(speed) * 3),
	coinsPerCredit(parameters.count("coinsPerCredit") ? parameters["coinsPerCredit"] : ""),
	mode(parameters.count("mode") ? parameters["mode"] : "")
{
	string linkedTriggers = parameters.count("linkedTriggers") ? parameters["linkedTriggers"] : "";
	// process list
	if (linkedTriggers.empty())
		throw Error("Missing linked triggers");

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
		uint mapIdx    = 0;
		bool firstItem = true;
		for (auto& mapId : groupMapIDs) {
			Utility::trim(mapId);
			// Convert mapId to trigger.
			uint16_t mapCode = Utility::parseNumber(mapId, "Unable to parse map ID, is not a number");
			string trigger;
			for (const auto& pair : itemsMap) {
				if (pair.second->pos == mapCode) {
					trigger = pair.first;
					break;
				}
			}
			if (trigger.empty()) {
				throw Error("Ignoring invalid map ID " + mapId);
				continue;
			}
			tempRecord.emplace_back(std::move(Record{trigger, firstItem, itemsMap[trigger], nullptr}));
			if (firstItem) {
				firstItems.push_back(trigger);
				firstItem = false;
			}
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
