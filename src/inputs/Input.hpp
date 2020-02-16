/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Input.hpp
 * @since     May 8, 2019
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

#include "devices/Group.hpp"
#include "devices/Element.hpp"
#include "utility/Log.hpp"
#include "utility/Utility.hpp"

using LEDSpicer::Devices::Element;
using LEDSpicer::Devices::Group;

#ifndef INPUT_HPP_
#define INPUT_HPP_ 1

namespace LEDSpicer {
namespace Inputs {

/**
 * LEDSpicer::Inputs::Input
 */
class Input {

public:

	Input(umap<string, string>& parameters) {}

	virtual ~Input() {}

	/**
	 * Sets the controlled elements and groups.
	 * @param controlledElements
	 * @param controlledGroups
	 */
	static void setInputControllers(
		umap<string, Element::Item*>* controlledElements,
		umap<string, Group::Item*>* controlledGroups
	);

	/**
	 * Draws the input configuration.
	 */
	virtual void drawConfig();

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

	/**
	 * Sets Initial mapping data.
	 * @param elementMap
	 * @param groupMap
	 */
	void setMaps(umap<string, Element::Item>& elementMap, umap<string, Group::Item>& groupMap);

protected:

	/// Output list of mapped elements by trigger + element name.
	static umap<string, Element::Item*>* controlledElements;

	/// Output list of mapped groups by trigger + group name.
	static umap<string, Group::Item*>* controlledGroups;

	/// Profile specific map. trigger -> Item.
	umap<string, Element::Item> elementMap;

	/// Profile specific settings. trigger -> Item.
	umap<string, Group::Item> groupMap;
};

// The functions to create and destroy inputs.
#define inputFactory(plugin) \
	extern "C" LEDSpicer::Inputs::Input* createInput(umap<string, string>& parameters) {return new plugin(parameters);} \
	extern "C" void destroyInput(LEDSpicer::Inputs::Input* instance) {delete instance;}

} /* namespace Inputs */
} /* namespace LEDSpicer */

#endif /* INPUT_HPP_ */
