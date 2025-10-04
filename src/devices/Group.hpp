/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Group.hpp
 * @since     Jun 22, 2018
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

#include "Element.hpp"

#pragma once

namespace LEDSpicer::Devices {

/**
 * LEDSpicer::Devices::Group
 * A group of elements.
 */
class Group {

public:

	Group(const Color& defaultColor) : defaultColor(defaultColor) {}

	Group(
		const string& name,
		const Color& defaultColor
	) : name(name), defaultColor(defaultColor) {}

	virtual ~Group() = default;

	/**
	 * Structure to handle groups with properties.
	 */
	struct Item : public Items {

		Group* const group;

		Item() = delete;

		/**
		 * Constructs an Item for a Group with specified properties.
		 * @param group Pointer to the associated Group.
		 * @param color Pointer to the desired color.
		 * @param filter Color filter to apply.
		 * @param pos Position in a list (default 0).
		 */
		Item(Group* group, const Color* color, Color::Filters filter, uint16_t pos = 0) :
			Items(color, filter, pos),
			group(group) {}

		Item(const Item& item) : Items(item), group(item.group) {}

		Item(Item&& item) : Items(item), group(std::move(item.group)) {}

		virtual ~Item() = default;

		string getName() const override {
			return group->getName();
		}

		void process(uint8_t percent, Color::Filters* filterOverride) const override {
			for (auto& e : group->getElements())
				e->setColor(*color, filterOverride ? *filterOverride : filter, percent);
		}
	};

	void drawElements();

	/**
	 * @return the number of elements.
	 */
	uint16_t size() const;

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
	Element* getElement(uint16_t index) const;

	/**
	 * @return the group name.
	 */
	const string& getName() const;

	/**
	 * @return the default color for this element.
	 */
	const Color& getDefaultColor() const;

	/**
	 * @return a reference to the hardware internal LEDs for all elements.
	 */
	const vector<uint8_t*>& getLeds() const;

	/// Stores groups by name.
	static unordered_map<string, Group> layout;

protected:

	/// Elements on this group.
	vector<Element*> elements;

	/// Cache all LED pointers here.
	vector<uint8_t*> leds;

	/// Stores the name.
	string name = "";

	/// Stores the default color.
	const Color& defaultColor;

};

using GroupPtrUMap  = unordered_map<string, Group*>;
using GroupItemUMap = unordered_map<string, Group::Item>;

} // namespace
