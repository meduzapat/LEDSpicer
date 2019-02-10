/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      PacLed64.hpp
 * @since     Feb 2, 2019
 * @author    Patricio A. Rossi (MeduZa)
 * @copyright Copyright © 2019 Patricio A. Rossi (MeduZa)
 */

#ifndef PACLED64_HPP_
#define PACLED64_HPP_ 1

#include "Ultimarc.hpp"

#define PAC_LED64_NAME       "Ultimarc PacLed 64"
#define PAC_LED64_PRODUCT    0x1401
#define PAC_LED64_WVALUE     0x0200
#define PAC_LED64_INTERFACE  1
#define PAC_LED64_LEDS       64
#define PAC_LED64_MAX_BOARDS 4

namespace LEDSpicer {
namespace Devices {
namespace Ultimarc {

/**
 * LEDSpicer::Devices::Ultimarc::PacLed64
 */
class PacLed64: public Ultimarc {

public:

	PacLed64();

	/**
	 * @param boardId
	 */
	PacLed64(uint8_t boardId, umap<string, string>& options) :
	Ultimarc(
		PAC_LED64_WVALUE,
		0, // to be defined.
		PAC_LED64_LEDS,
		PAC_LED64_MAX_BOARDS,
		boardId,
		PAC_LED64_NAME
	) {}

	virtual ~PacLed64() {}

	virtual void drawHardwarePinMap();

	void transfer();

	uint16_t getProduct();

protected:

	virtual void afterConnect() {}

	virtual void afterClaimInterface();
};

} /* namespace Ultimarc */
} /* namespace Devices */
} /* namespace LEDSpicer */

deviceFactory(LEDSpicer::Devices::Ultimarc::PacLed64)

#endif /* PACLED64_HPP_ */
