/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Input.cpp
 * @since     May 8, 2019
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

#include "Input.hpp"

using namespace LEDSpicer::Inputs;

umap<string, LEDSpicer::Devices::Element::Item*>* Input::controlledElements = nullptr;
umap<string, LEDSpicer::Devices::Group::Item*>* Input::controlledGroups     = nullptr;

void Input::setInputControllers(
	umap<string, Element::Item*>* controlledElements,
	umap<string, Group::Item*>* controlledGroups
) {
	Input::controlledElements = controlledElements;
	Input::controlledGroups   = controlledGroups;
}

void Input::drawConfig() {

	if (elementMap.size()) {
		cout << "Element mappings:" << endl;
		for (auto& e : elementMap)
			cout <<
				"Target element: " << e.second.element->getName() << endl <<
				"Trigger: " << e.first << endl <<
				"Color: " << e.second.color->getName() << endl <<
				"Filter: " << Color::filter2str(e.second.filter) << endl << endl;
	}

	if (groupMap.size()) {
		cout << "Group mappings:" << endl;
		for (auto& g : groupMap)
			cout <<
				"Target group: " << g.second.group->getName() << endl <<
				"Trigger: " << g.first << endl <<
				"Color: " << g.second.color->getName() << endl <<
				"Filter: " << Color::filter2str(g.second.filter) << endl << endl;
	}
}

void Input::setMaps(umap<string, Element::Item>& elementMap, umap<string, Group::Item>& groupMap) {
	this->elementMap = std::move(elementMap);
	this->groupMap   = std::move(groupMap);
}
