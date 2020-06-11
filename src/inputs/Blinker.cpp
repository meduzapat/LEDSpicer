/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Blinker.cpp
 * @since     Aug 17, 2019
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

#include "Blinker.hpp"

using namespace LEDSpicer::Inputs;

void Blinker::process() {

	readAll();

	for (auto& e :itemsToClean)
		blinkingItems.erase(e);

	itemsToClean.clear();

	blink();

	if (not events.size())
		return;

	for (auto& event : events) {

		if (not event.value)
			continue;

		string codeName = std::to_string(event.code);
		if (itemsMap.count(codeName)) {
			LogDebug("key: " + codeName + " adds: " + to_string(times) + " times to element: " + itemsMap[codeName]->getName());
			// switch
			if (blinkingItems.count(event.code)) {
				blinkingItems[event.code].times = 0;
			}
			else {
				blinkingItems.emplace(event.code, Times{itemsMap[codeName], 0});
			}
		}
	}
}

void Blinker::activate() {
	controlledItems->clear();
	Reader::activate();
}

void Blinker::deactivate() {
	blinkingItems.clear();
	Reader::deactivate();
}

void Blinker::drawConfig() {
	Reader::drawConfig();
	cout << endl << "Stop After " << times << " times" << endl;
	Speed::drawConfig();
}

void Blinker::blink() {
	if (frames == cframe) {
		cframe = 0;
		for (auto& i : blinkingItems) {
			string name = std::to_string(i.first);
			// deactivate after time passed.
			if (i.second.times == times) {
				controlledItems->erase(name);
				itemsToClean.push_back(i.first);
				continue;
			}
			if (on) {
				controlledItems->erase(name);
			}
			else {
				controlledItems->emplace(name, i.second.item);
				++i.second.times;
			}
		}
		on = not on;
	}
	else {
		++cframe;
	}
}
