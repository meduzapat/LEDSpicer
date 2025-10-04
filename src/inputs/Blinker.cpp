/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Blinker.cpp
 * @since     Aug 17, 2019
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

#include "Blinker.hpp"

using namespace LEDSpicer::Inputs;

inputFactory(Blinker)

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

		if (itemsUMap.exists(event.trigger)) {
			LogDebug("key: " + event.trigger + " adds: " + to_string(times) + " times to element: " + itemsUMap[event.trigger]->getName());
			// switch
			if (blinkingItems.exists(event.trigger))
				blinkingItems[event.trigger].times = 0;
			else
				blinkingItems.emplace(event.trigger, Times{itemsUMap[event.trigger], 0});
		}
	}
}

void Blinker::deactivate() {
	blinkingItems.clear();
	Reader::deactivate();
}

void Blinker::drawConfig() const {
	cout << SEPARATOR << endl << "Type: Blinker" << endl;
	Reader::drawConfig();
	cout << "Stop After " << to_string(times) << " times" << endl;
	Speed::drawConfig();
}

void Blinker::blink() {
	if (frames == cframe) {
		cframe = 0;
		for (auto& i : blinkingItems) {
			// deactivate after time passed.
			if (i.second.times == times) {
				controlledItems.erase(i.first);
				itemsToClean.push_back(i.first);
				continue;
			}
			if (on) {
				controlledItems.erase(i.first);
			}
			else {
				controlledItems.emplace(i.first, i.second.item);
				++i.second.times;
			}
		}
		on = not on;
	}
	else {
		++cframe;
	}
}
