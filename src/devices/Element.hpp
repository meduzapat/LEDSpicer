/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Element.hpp
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

// For ints.
#include <cstdint>

// To handle dynamic arrays.
#include <vector>
using std::vector;

#include "utility/Color.hpp"
#include "utility/Error.hpp"

#ifndef ELEMENT_HPP_
#define ELEMENT_HPP_ 1

#define SINGLE_PIN 0

namespace LEDSpicer {
namespace Devices {


/**
 * LEDSpicer::Devices::Element
 * Represent an Element that contain pins (LEDs), with functionality to handle them.
 */
class Element {

public:

	Element(const string& name, uint8_t* pin);

	Element(const string& name, uint8_t* pinR, uint8_t* pinG, uint8_t* pinB);

	Element(Element* other);

	/**
	 * Structure for elements with properties.
	 */
	struct Item {
		Element* element      = nullptr;
		const Color* color    = nullptr;
		Color::Filters filter = Color::Filters::Normal;
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
	 * Covert the pin values into color.
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

protected:

	/**
	 * Array of pointers to the original pins
	 */
	vector<uint8_t*> pins;

	/// keeps the element name.
	string name;

};

}} /* namespace LEDSpicer */

#endif /* ELEMENT_HPP_ */
