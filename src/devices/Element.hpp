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

#define SINGLE_LED 0

namespace LEDSpicer::Devices {

/**
 * Helper base class to unify elements and groups.
 */
struct Items {

	/// Stores the position in a list if necessary.
	const uint16_t pos;

	/// Stores the desired color.
	const Color* color;

	/// Stores the filter to be used when applying the color.
	const Color::Filters filter;

	Items() = delete;
	Items(const Color* color, Color::Filters filter, uint16_t pos) : color(color), filter(filter), pos(pos) {}
	Items(const Items& item)  : color(item.color), filter(item.filter), pos(item.pos) {}
	Items(Items&& item) : color(std::move(item.color)), filter(std::move(item.filter)), pos(std::move(item.pos)) {}

	virtual ~Items() = default;

	virtual string getName() const = 0;
	virtual void process(uint8_t percent, Color::Filters* filterOverride) const = 0;
};

/**
 * LEDSpicer::Devices::Element
 * Represent an Element that handles LEDs, single color or RGB, solenoids or motors.
 * Each LEDs value is stored into the hardware that owns this element.
 */
class Element {

public:

	/**
	 * Creates a new monochrome Element.
	 * @param name Element name.
	 * @param led pointer to the LED on the hardware.
	 * @param defaultColor
	 */
	Element(
		const string& name,
		uint8_t* led,
		const Color& defaultColor,
		uint timeOn,
		uint8_t brightness
	) :
		name(name),
		leds{led},
		defaultColor(defaultColor),
		timeOn(timeOn),
		// Only calculate brightness of not 0 or 100.
		brightness(brightness)
	{}

	/**
	 * Creates a new RGB Element.
	 * @param name
	 * @param ledR pointer for the Red LED
	 * @param ledG pointer for the Green LED
	 * @param ledB pointer for the Blue LED
	 * @param defaultColor
	 */
	Element(
		const string& name,
		uint8_t* ledR,
		uint8_t* ledG,
		uint8_t* ledB,
		const Color& defaultColor,
		uint8_t brightness
	) :
		name(name),
		leds{ledR, ledG, ledB},
		defaultColor(defaultColor),
		// Only calculate brightness of not 0 or 100.
		brightness(brightness)
	{}

	/**
	 * Copy from other Element.
	 * @param other
	 */
	Element(Element* other) :
		name(other->name),
		leds(other->leds.size(), nullptr),
		defaultColor(other->defaultColor),
		brightness(other->brightness)
	{}

	/**
	 * Structure for elements with properties.
	 */
	struct Item : public Items {

		Element* const element;

		Item() = delete;

		Item(Element* element, const Color* color, Color::Filters filter, uint16_t pos = 0) :
			Items(color, filter, pos),
			element(element) {}

		Item(const Item& item) : Items(item), element(item.element) {}

		Item(Item&& item) : Items(item), element(std::move(item.element)) {}

		virtual ~Item() = default;

		string getName() const override {
			return element->getName();
		}

		void process(uint8_t percent, Color::Filters* filterOverride) const override {
			element->setColor(*color, filterOverride ? *filterOverride : filter, percent);
		}
	};

	virtual ~Element() = default;

	/**
	 * Replaces the LED values with a new color.
	 * @param color
	 */
	void setColor(const Color& color);

	/**
	 * Replaces the led values with a a new color with a filter.
	 * @param color
	 * @param filter
	 */
	void setColor(const Color& color, const Color::Filters& filter, uint8_t percent = 50);

	/**
	 * Convert the LED values into color.
	 * @return
	 */
	Color getColor() const;

	/**
	 * Set a single led value.
	 * @param led pointer to the led in the hardware.
	 * @param value
	 */
	void setLedValue(uint16_t led, uint8_t value);

	/**
	 * Return the LED value.
	 * @param led
	 * @return
	 */
	uint8_t getLedValue(uint16_t led) const;

	/**
	 * Returns a pointer to the LED value.
	 * @param led
	 * @return
	 */
	uint8_t* const getLed(uint16_t led) const;

	/**
	 * Returns a reference to the internal led(s).
	 * @return a reference to the hardware internal LEDs.
	 */
	const vector<uint8_t*>& getLeds() const;

	/**
	 * Returns the number of LEDs.
	 * @return
	 */
	uint8_t size() const;

	/**
	 * Returns the element's name.
	 * @return
	 */
	const string getName() const;

	/**
	 * Returns the default color for this element.
	 * @return
	 */
	const Color& getDefaultColor() const;

	/**
	 * @return true if this element turns itself off after several ms.
	 */
	bool isTimed() const;

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
	const string name;

	/// Array of pointers to the original LEDs on the hardware.
	const vector<uint8_t*> leds;

	/// Color used for the craft profile elements.
	const Color& defaultColor;

	/// If bigger than zero, the LED will go OFF after that time (in ms)
	const uint16_t timeOn = 0;

	/// Custom Brightness 1 to 99.
	const uint8_t brightness;

	/// A point in time to know when the timeOn is completed.
	system_clock::time_point clockTime;
};

} /* namespace */

#endif /* ELEMENT_HPP_ */
