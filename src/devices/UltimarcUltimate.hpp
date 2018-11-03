/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      UltimarcUltimate.hpp
 * @since     Jun 23, 2018
 * @author    Patricio A. Rossi (MeduZa)
 * @copyright Copyright © 2018 Patricio A. Rossi (MeduZa)
 */

// For special io format
#include <iomanip>

#include "Ultimarc.hpp"

#ifndef ULTIMARCULTIMATE_HPP_
#define ULTIMARCULTIMATE_HPP_ 1

#define IPAC_ULTIMATE "IPAC_ULTIMATE"

#define IPAC_ULTIMATE_PRODUCT       0x0410
#define IPAC_ULTIMATE_VALUE         0x0203
#define IPAC_ULTIMATE_INTERFACE     3
#define IPAC_ULTIMATE_NGC_INTERFACE 2
#define IPAC_ULTIMATE_LEDS          96

namespace LEDSpicer {
namespace Devices {

/**
 * LEDSpicer::Devices::UltimarcUltimate
 * Class to keep Ultimate I/O data and functionality.
 */
class UltimarcUltimate: public Ultimarc {

public:

	/**
	 * @param boardId
	 */
	UltimarcUltimate(uint8_t boardId, umap<string, string>& options);

	virtual ~UltimarcUltimate();

	virtual void drawHardwarePinMap();

	void transfer();

protected:

	virtual void afterConnect();

	virtual void afterClaimInterface();

};

}} /* namespace LEDSpicer */

#endif /* ULTIMARCULTIMATE_HPP_ */
