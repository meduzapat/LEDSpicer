/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Element.hpp
 * @since     Jun 22, 2018
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2018 - 2024 Patricio A. Rossi (MeduZa)
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

// For ints.
#include <cstdint>

// To handle dynamic arrays.
#include <vector>
using std::vector;

#include "utility/Color.hpp"
#include "utility/Error.hpp"

#include <chrono>
using std::chrono::system_clock;

#ifndef ELEMENT_HPP_
#define ELEMENT_HPP_ 1

#define SINGLE_PIN 0

namespace LEDSpicer::Devices {

/**
 * Helper base class to unify elements and groups.
 */
struct Items {

	/// Stores the position in a list if necessary.
	uint pos = 0;

	/// Stores the desired color.
	const Color* color = nullptr;

	/// Stores the filter to be used when applying the color.
	Color::Filters filter = Color::Filters::Normal;

	Items() = default;
	Items(const Color* color, Color::Filters filter, uint pos) : color(color), filter(filter), pos(pos) {}
	Items(const Items& item)  : color(item.color), filter(item.filter), pos(item.pos) {}
	Items(Items&& item) : color(std::move(item.color)), filter(std::move(item.filter)), pos(std::move(item.pos)) {}

	virtual ~Items() = default;

	virtual string getName() const = 0;
	virtual void process() const   = 0;
};

/**
 * LEDSpicer::Devices::Element
 * Represent an Element that contain pins (LEDs), with functionality to handle them.
 */
class Element {

public:

	/**
	 * Creates a new monochrome Element.
	 * @param name
	 * @param pin
	 * @param defaultColor
	 */
	Element(
		const string& name,
		uint8_t* pin,
		const Color& defaultColor,
		uint timeOn,
		uint8_t brightness
	);

	/**
	 * Creates a new RGB Element.
	 * @param name
	 * @param pinR
	 * @param pinG
	 * @param pinB
	 * @param defaultColor
	 */
	Element(
		const string& name,
		uint8_t* pinR,
		uint8_t* pinG,
		uint8_t* pinB,
		const Color& defaultColor,
		uint8_t brightness
	);

	/**
	 * Copy from other Element.
	 * @param other
	 */
	Element(Element* other);

	/**
	 * Structure for elements with properties.
	 */
	struct Item : public Items {

		Element* element = nullptr;

		Item() = default;

		Item(Element* element, const Color* color, Color::Filters filter, uint pos = 0) :
			Items(color, filter, pos),
			element(element) {}

		Item(const Item& item) : Items(item), element(item.element) {}

		Item& operator=(const Item& item) {
			element = item.element;
			color   = item.color;
			filter  = item.filter;
			pos     = item.pos;
			return *this;
		}

		Item(Item&& item) : Items(item), element(std::move(item.element)) {}

		virtual ~Item() = default;

		string getName() const {
			return element->getName();
		}

		void process() const  {
			element->setColor(*color, filter);
		}
	};

	virtual ~Element() = default;

	/**
	 * Replaces the pin values with a new color.
	 * @param color
	 */
	void setColor(const Color& color);

	/**
	 * Replaces the pin values with a a new color with a filter.
	 * @param color
	 * @param filter
	 */
	void setColor(const Color& color, const Color::Filters& filter, uint8_t percent = 50);

	/**
	 * Convert the pin values into color.
	 * @return
	 */
	Color getColor();

	/**
	 * Set a single pin value.
	 * @param pinNumber
	 * @param val
	 */
	void setPinValue(uint8_t pinNumber, uint8_t val);

	/**
	 * Links a new pin,
	 * first will be red (or intensity when is only a single pin),
	 * second green, and third blue.
	 * @param val
	 */
	void linkPin(uint8_t* val);

	/**
	 * Return the pin value.
	 * @param pinNumber
	 * @return
	 */
	uint8_t getPinValue(uint8_t pinNumber) const;
	/**
	 * Returns a pointer to the pin value.
	 * @param pinNumber
	 * @return
	 */
	uint8_t* const getPin(uint8_t pinNumber) const;

	/**
	 * Returns a reference to the internal pin(s).
	 * @return
	 */
	const vector<uint8_t*>& getPins() const;

	/**
	 * Returns the number of pins.
	 * @return
	 */
	uint8_t size() const;

	/**
	 * Returns the element's name.
	 * @return
	 */
	string getName();

	/**
	 * Returns the default color for this element.
	 * @return
	 */
	const Color& getDefaultColor();

	/**
	 * @return true if this element turns itself off after several ms.
	 */
	bool isTimed();

	/**
	 * Check if the time is over and turn the element off.
	 */
	void checkTime();

	/**
	 * Draws information about this element.
	 */
	void draw();

protected:

	/// Keeps the element name.
	string name;

	/// Array of pointers to the original pins.
	vector<uint8_t*> pins;

	/// Color used for the craft profile elements.
	const Color& defaultColor;

	/// If bigger than zero, the pin will go OFF after that time (in ms)
	uint16_t timeOn = 0;

	/// Custom Brightness 1 to 99.
	uint8_t brightness;

	/// A pint in time to know when the timeOn is completed.
	system_clock::time_point clockTime;
};

} /* namespace */

#endif /* ELEMENT_HPP_ */
