/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Device.hpp
 *
 * @since     Jun 7, 2018
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

#include "utility/Hardware.hpp"
#include "Group.hpp"

#ifndef DEVICE_HPP_
#define DEVICE_HPP_ 1

namespace LEDSpicer::Devices {

/**
 * LEDSpicer::Devices::Device
 * Generic Device settings and functionality.
 * check XXX_TRANSFER definitions, 0 all, 1 individual, 2 batches.
 * Right now only raspberry pi uses this feature.
 */
class Device : public Hardware {

public:

	Device(uint8_t elements, const string& name);

	virtual ~Device() = default;

	/**
	 * Prepares the device.
	 */
	void initialize();

	/**
	 * Terminates the device.
	 */
	void terminate();

	/**
	 * This method will be called every time a pack of data is ready for transfer.
	 */
	virtual void transfer() const = 0;

	/**
	 * Set a LED to an intensity
	 * @param led
	 * @param intensity (0-255)
	 * @return
	 */
	Device* setLed(uint8_t led, uint8_t intensity);

	/**
	 * Set all the LEDs to an specific intensity
	 * @param intensity (0-255)
	 * @return
	 */
	Device* setLeds(uint8_t intensity);

	/**
	 * Returns a pointer to a single LED.
	 * @param ledPos
	 * @return
	 */
	uint8_t* getLed(uint8_t ledPos);

	/**
	 * Register a new Element with a single LED.
	 * @param name
	 * @param led
	 * @param defaultColor
	 */
	void registerElement(
		const string& name,
		uint8_t led,
		const Color& defaultColor,
		uint timeOn,
		uint8_t brightness
	);

	/**
	 * Register a new Element with three LEDs (RGB).
	 * @param name
	 * @param led1
	 * @param led2
	 * @param led3
	 * @param defaultColor
	 */
	void registerElement(
		const string& name,
		uint8_t led1,
		uint8_t led2,
		uint8_t led3,
		const Color& defaultColor,
		uint8_t brightness
	);

	/**
	 * Returns a pointer to an element using the name.
	 * @param name
	 * @return
	 */
	Element* getElement(const string& name);

	/**
	 * Check if a LED (or pin) is valid.
	 * @param led
	 * @throw Error if not valid.
	 */
	void validateLed(uint8_t led) const;

	/**
	 * Returns the number of registered elements.
	 * @return
	 */
	uint8_t getNumberOfElements() const;

	/**
	 * Returns all elements mapped by name.
	 * @return
	 */
	umap<string, Element>* getElements();

	/**
	 * Sets all LEDs off.
	 */
	virtual void resetLeds();

	/**
	 * Populates the pins with the correct pin number used by elements and
	 * displays the pin in a similar way they are found on the hardware.
	 */
	virtual void drawHardwarePinMap() = 0;

	/**
	 * Returns the number of LEDs (pins) this board controls.
	 * @return
	 */
	uint8_t getNumberOfLeds() const;

	/**
	 * Pack the data into the device.
	 */
	virtual void packData();

protected:

	/// Device LEDs (pins)
	vector<uint8_t> LEDs;

	/// Copy of device LEDs (pins)
	vector<uint8_t> oldLEDs;

	/// Maps elements by name.
	umap<string, Element> elementsByName;

};

// The functions to create and destroy devices.
#define deviceFactory(plugin) \
	extern "C" Device* createDevice(umap<string, string>& options) { return new plugin(options); } \
	extern "C" void destroyDevice(Device* instance) { delete instance; }

} /* namespace */

#endif /* DEVICE_HPP_ */
