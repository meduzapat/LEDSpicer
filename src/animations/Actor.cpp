/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Actor.cpp
 * @since     Jun 22, 2018
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2018 - 2019 Patricio A. Rossi (MeduZa)
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

#include "Actor.hpp"

using namespace LEDSpicer::Animations;

uint8_t Actor::FPS = 0;

Actor::Actor(
	umap<string, string>& parameters,
	Group* const group,
	const vector<string>& requiredParameters
) :
	filter(Color::str2filter(parameters["filter"])),
	group(group)
{

	affectedElements.resize(group->size(), false);
	affectedElements.shrink_to_fit();
	Utility::checkAttributes(requiredParameters, parameters, "actor.");

	restart();
}

bool Actor::draw() {
	affectAllElements();
	calculateElements();
	if (not affectedElements.empty())
		switch (filter) {
		case Color::Filters::Mask:
			// turn off any other element.
			for (uint8_t elIdx = 0; elIdx < group->size(); ++elIdx) {
				if (affectedElements[elIdx])
					continue;
				changeElementColor(elIdx, Color::getColor("Black"), Color::Filters::Normal, 100);
			}
		}
	return not running();
}

void Actor::drawConfig() {
	cout <<
		"Group: " << group->getName() << endl <<
		"Filter: " << Color::filter2str(filter) << endl;
}

void Actor::setFPS(uint8_t FPS) {
	LogDebug("Setting FPS to " + to_string(FPS));
	Actor::FPS = FPS;
}

uint8_t Actor::getFPS() {
	return FPS;
}

uint8_t Actor::getNumberOfElements() const {
	return group->getElements().size();
}

void Actor::changeElementColor(uint8_t index, const Color& color, Color::Filters filter, uint8_t percent) {
	affectedElements[index] = true;
	Element* e = group->getElement(index);
	e->setColor(*e->getColor().set(color, filter, percent));
}

void Actor::changeElementsColor(const Color& color, Color::Filters filter, uint8_t percent) {
	affectAllElements(true);
	for (uint8_t c = 0; c < group->size(); ++c)
		changeElementColor(c, color, filter, percent);
}

void Actor::affectAllElements(bool value) {
	affectedElements.assign(group->size(), value);
}

bool Actor::isElementAffected(uint8_t index) {
	return affectedElements[index];
}
