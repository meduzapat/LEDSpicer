/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Input.hpp
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

#include "devices/Group.hpp"
#include "devices/Element.hpp"
#include "utilities/Log.hpp"
#include "utilities/Utility.hpp"

using namespace LEDSpicer::Devices;
using namespace LEDSpicer::Utilities;

#pragma once

// The functions to create and destroy inputs.
#define inputFactory(plugin) \
	extern "C" Input* createInput(StringUMap& parameters, ItemPtrUMap& inputMaps) {return new plugin(parameters, inputMaps);} \
	extern "C" void destroyInput(Input* instance) {delete instance;}

namespace LEDSpicer::Inputs {

/**
 * LEDSpicer::Inputs::Input
 */
class Input {

public:

	Input(
		StringUMap&,
		ItemPtrUMap& inputMaps
	) : itemsUMap(std::move(inputMaps)) {}

	virtual ~Input();

	/**
	 * Returns a list of controlled items.
	 * @return
	 */
	static const ItemPtrUMap& getControlledInputs();

	/**
	 * Removes all input registered inputs.
	 */
	static void clearControlledInputs();

	/**
	 * Draws the input configuration.
	 */
	virtual void drawConfig() const;

	/**
	 * Called when a profile is activated.
	 * @return
	 */
	virtual void activate() = 0;

	/**
	 * Called when a profile is deactivated.
	 * @return
	 */
	virtual void deactivate() = 0;

	/**
	 * Called every frame.
	 */
	virtual void process() = 0;

protected:

	/// List of elements that need to be Output, mapped items by trigger.
	static ItemPtrUMap controlledItems;

	/// Input specific map. trigger -> Item.
	ItemPtrUMap itemsUMap;

	/**
	 * @param name
	 * @return string, the map using the element or group name.
	 */
	string findItemMapByName(string& name);

	/**
	 * Attempts to remove an item by its trigger.
	 * @param trigger trigger to remove
	 * @return true if the item was removed, false instead.
	 */
	static bool removeControlledItemByTrigger(const string& trigger);

};

using InputPtrUMap = unordered_map<string, Input*>;
using InputUMap    = unordered_map<string, Input>;

} // namespace
