/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Input.cpp
 * @since     May 8, 2019
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

#include "Input.hpp"

using namespace LEDSpicer::Inputs;

ItemPtrUMap Input::controlledItems;

Input::~Input() {
	LogDebug("Releasing input maps...");
	for (auto& p : itemsUMap)
		delete p.second;
}

const ItemPtrUMap& Input::getControlledInputs() {
	return Input::controlledItems;
}

void Input::clearControlledInputs() {
	Input::controlledItems.clear();
}

void Input::drawConfig() const {

	if (not itemsUMap.size())
		return;

	cout << "Elements and Groups mapping:" << endl;
	for (auto& e : itemsUMap)
		cout <<
			"Target: "  << e.second->getName()                 << endl <<
			"Trigger: " << e.first                             << endl <<
			"Color: "   << e.second->color->getName()          << endl <<
			"Filter: "  << Color::filter2str(e.second->filter) << endl << endl;
}

string Input::findItemMapByName(string& name) {
	for (auto& eMap : itemsUMap) {
		if (eMap.second->getName() == name)
			return eMap.first;
	}
	throw Error("Unable to find item named ") << name;
}

bool Input::removeControlledItemByTrigger(const string& trigger) {
	if (controlledItems.exists(trigger)) {
		controlledItems.erase(trigger);
		return true;
	}
	return false;
}
