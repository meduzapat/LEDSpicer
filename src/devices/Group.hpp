/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Group.hpp
 * @since     Jun 22, 2018
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

#include "Element.hpp"

#ifndef LAYOUT_HPP_
#define LAYOUT_HPP_ 1

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

	/**
	 * Structure to handle groups with properties.
	 */
	struct Item : public Items {

		Group* group = nullptr;

		Item() = default;

		Item(Group* group, const Color* color, Color::Filters filter) :
			Items(color, filter),
			group(group) {}

		Item(const Item& item) : Items(item), group(item.group) {}

		Item(Item&& item) : Items(item), group(std::move(item.group)) {}

		Item& operator=(const Item& item) {
			group     = item.group;
			color     = item.color;
			filter    = item.filter;
			return *this;
		}

		virtual ~Item() = default;

		virtual string getName() const {
			return group->getName();
		}

		void process() const {
			for (auto& e : group->getElements())
				e->setColor(*color, filter);
		}
	};

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

	/**
	 * Returns the default color for this element.
	 * @return
	 */
	const Color& getDefaultColor();

protected:

	/// Elements on this group.
	vector<Element*> elements;

	string name = "";

	const Color& defaultColor;

};

} /* namespace */

#endif /* LAYOUT_HPP_ */
