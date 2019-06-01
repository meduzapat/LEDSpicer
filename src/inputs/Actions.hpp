/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Actions.hpp
 * @since     May 23, 2019
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

#ifndef ACTIONS_HPP_
#define ACTIONS_HPP_ 1

#include "utility/Speed.hpp"
#include "Reader.hpp"

namespace LEDSpicer {
namespace Inputs {

/**
 * LEDSpicer::Inputs::Actions
 *
 * This plugin will map actions to buttons to recreate an interaction with the controls
 */
class Actions: public Reader, public Speed {

public:

	Actions(umap<string, string>& parameters) :
		Reader(parameters),
		Speed(parameters.count("speed") ? parameters["speed"] : ""),
		frames(static_cast<uint8_t>(speed) * 3),
		linkedElementsRaw(parameters.count("linkedElements") ? parameters["linkedElements"] : "") {}

	virtual ~Actions() = default;

	virtual void process();

	virtual void activate();

	virtual void deactivate();

protected:

	uint8_t
		frames,
		cframe = 0;

	bool on = false;

	// Keeps the list of linked elements.
	string linkedElementsRaw;

	vector<string> preOnElements;
	vector<string> preOnGroups;

	umap<string, Element::Item*> blinkingElements;
	umap<string, Group::Item*> blinkingGroups;

	/// Maps element names to triggers.
	umap<string, string> linkedElements;

	/// Maps group names to triggers.
	umap<string, string> linkedGroup;

	void blink();

	string findElementMapByName(string& name);

};

} /* namespace Inputs */
} /* namespace LEDSpicer */

inputFactory(LEDSpicer::Inputs::Actions)

#endif /* ACTIONS_HPP_ */
