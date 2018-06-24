/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file		Device.hpp
 * @ingroup
 * @since		Jun 7, 2018
 * @author		Patricio A. Rossi (MeduZa)
 * @copyright	Copyright © 2018 Patricio A. Rossi (MeduZa)
 */

#ifndef DEVICE_HPP_
#define DEVICE_HPP_ 1

#include "../ConnectionUSB.hpp"

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

	Device() : ConnectionUSB(UM_REQUEST_TYPE, UM_REQUEST) {}

	virtual ~Device() {}

	Device* setLed(uint8_t led, uint8_t intensity);

	Device* setLeds(uint8_t intensity);

	string getName();

	uint8_t getNumberOfLeds();

	/**
	 * Populates the pins with the correct pin number and
	 * displays the pin in a similar way they are found on the hardware.
	 */
	virtual void drawHardwarePinMap() = 0;

// Development debugging functions
#ifdef DEVELOP

	/**
	 * Prints the board technical information.
	 */
	void printDeviceInformation();

#endif

protected:

};

}} /* namespace LEDSpicer */

#endif /* DEVICE_HPP_ */
