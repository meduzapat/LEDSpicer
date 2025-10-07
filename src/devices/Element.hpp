/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Element.hpp
 * @since     Jun 22, 2018
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

#include "Items.hpp"
#include "utilities/Time.hpp"

#pragma once

#define SINGLE_LED 0

namespace LEDSpicer::Devices {

using namespace LEDSpicer::Utilities;

/**
 * LEDSpicer::Devices::Element
 * Represent an Element that handles LEDs, single color or RGB, solenoids or motors.
 * Each LEDs value is stored into the hardware that owns this element.
 */
class Element : public Time {

public:

	/**
	 * Creates a new monochrome Element.
	 * @param name Element name.
	 * @param led pointer to the LED on the hardware.
	 * @param defaultColor
	 * @param timeOn the number of milliseconds to say ON, used for solenoids.
	 * @param brightness The intensity of modifier for this Element.
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
		brightness(brightness)
	{}

	/**
	 * Creates a new RGB Element.
	 * @param name
	 * @param ledR pointer for the Red LED
	 * @param ledG pointer for the Green LED
	 * @param ledB pointer for the Blue LED
	 * @param defaultColor
	 * @param brightness The intensity of modifier for this Element.
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
		brightness(brightness)
	{}

	/**
	 * Creates a new Element that may control multiple RGB LEDs.
	 * @param name Element name.
	 * @param leds reference to the LEDs on the hardware, is multiple of 3 for RGB.
	 * @param defaultColor
	 * @param brightness The intensity of modifier for this Element.
	 */
	Element(
		const string& name,
		const vector<uint8_t*>& leds,
		const Color& defaultColor,
		uint8_t brightness
	) :
		name(name),
		leds(leds),
		defaultColor(defaultColor),
		brightness(brightness)
	{}

	/**
	 * Structure for elements with properties.
	 */
	struct Item : public Items {

		Element* const element;

		Item() = delete;

		/**
		 * Constructs an Item for an Element with specified properties.
		 * @param element Pointer to the associated Element.
		 * @param color Pointer to the desired color.
		 * @param filter Color filter to apply.
		 * @param pos Position in a list (default 0).
		 */
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
	uint8_t* getLed(uint16_t led) const;

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
	 * @returns true if the time is running false if not.
	 */
	void checkTime();

	/**
	 * Draws information about this element.
	 */
	void draw() const;

	/// Global map of all elements, indexed by name, for lookup purposes.
	static unordered_map<string, Element*> allElements;

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

};

using ElementUMap     = unordered_map<string, Element>;
using ElementItemUMap = unordered_map<string, Element::Item>;

} // namespace
