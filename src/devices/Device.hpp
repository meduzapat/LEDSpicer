/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Device.hpp
 * @ingroup
 * @since     Jun 7, 2018
 * @author    Patricio A. Rossi (MeduZa)
 * @copyright Copyright © 2018 Patricio A. Rossi (MeduZa)
 */

#include "../ConnectionUSB.hpp"
#include "Group.hpp"

#ifndef DEVICE_HPP_
#define DEVICE_HPP_ 1

namespace LEDSpicer {
namespace Devices {

/**
 * LEDSpicer::Devices::Device
 * Generic Device settings and functionality.
 * Note: in the future if different brands or types of devices are
 * added, this file will need to be split into specialized Devices kinds.
 */
class Device : public ConnectionUSB {

public:

	using ConnectionUSB::ConnectionUSB;

	virtual ~Device() {}

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
	 */
	void registerElement(const string& name, uint8_t led);

	/**
	 * Register a new Element with three LEDs (RGB).
	 * @param name
	 * @param led1
	 * @param led2
	 * @param led3
	 */
	void registerElement(const string& name, uint8_t led1, uint8_t led2, uint8_t led3);

	Element* getElement(const string& name);

	/**
	 * Check if a LED (or pin) is valid.
	 * @param led
	 * @throw Error if not valid.
	 */
	void validateLed(uint8_t led) const;

	/**
	 * Returs the number of registered elements.
	 * @return
	 */
	uint8_t getNumberOfElements() const;

	umap<string, Element>* getElements();

protected:

	/// Maps elements by name.
	umap<string, Element> elementsByName;

	/// Basic
	void afterClaimInterface();

};

}} /* namespace LEDSpicer */

#endif /* DEVICE_HPP_ */
