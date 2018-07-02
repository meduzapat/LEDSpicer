/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file		UltimarcUltimate.hpp
 * @since		Jun 23, 2018
 * @author		Patricio A. Rossi (MeduZa)
 * @copyright	Copyright © 2018 Patricio A. Rossi (MeduZa)
 */

// For special io format
#include <iomanip>

#ifndef ULTIMARCULTIMATE_HPP_
#define ULTIMARCULTIMATE_HPP_ 1

#define IPAC_ULTIMATE "IPAC_ULTIMATE"

#define IPAC_ULTIMATE_VENDOR        0xD209
#define IPAC_ULTIMATE_PRODUCT       0x0410
#define IPAC_ULTIMATE_VALUE         0x0203
#define IPAC_ULTIMATE_INTERFACE     3
#define IPAC_ULTIMATE_NGC_INTERFACE 2
#define IPAC_ULTIMATE_LEDS          96

#include "Device.hpp"

namespace LEDSpicer {
namespace Devices {

/**
 * LEDSpicer::Devices::UltimarcUltimate
 * Class to keep Ultimate I/O data and functionality.
 */
class UltimarcUltimate: public Device {

public:

	/**
	 * @todo right now I have no way to test multiple boards, and no idea how to do it.
	 * @param boardId
	 * @param fps
	 */
	UltimarcUltimate(uint8_t boardId);

	virtual ~UltimarcUltimate();

	virtual void drawHardwarePinMap();

protected:

	virtual void afterConnect();

	virtual void afterClaimInterface();

};

}} /* namespace LEDSpicer */

#endif /* ULTIMARCULTIMATE_HPP_ */
