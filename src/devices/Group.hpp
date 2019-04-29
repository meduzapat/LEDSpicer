/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Group.hpp
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

#include "Element.hpp"

#ifndef LAYOUT_HPP_
#define LAYOUT_HPP_ 1

namespace LEDSpicer {
namespace Devices {

/**
 * LEDSpicer::Devices::Layout
 * A group of elements.
 */
class Group {

public:

	Group() = default;

	Group(const string& name) : name(name) {}

	virtual ~Group() = default;

	void drawElements();

	/**
	 * @return the number of elements.
	 */
	uint8_t size() const;

	/**
	 * Inserts a new element.
	 * @param element
	 */
	void linkElement(Element* element);

	/**
	 * Reduces the group elements to the minimum necessary.
	 */
	void shrinkToFit();

	/**
	 * @return a reference to the pointers to the internal elements.
	 */
	const vector<Element*>& getElements() const;

	/**
	 * @param index
	 * @return a reference to the internal element.
	 */
	Element* getElement(uint8_t index);

	/**
	 * @return the group name.
	 */
	const string& getName() const;

protected:

	/// Elements on this group.
	vector<Element*> elements;

	string name = "";

};

}} /* namespace LEDSpicer */

#endif /* LAYOUT_HPP_ */
