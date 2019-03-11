/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      LedWiz32.hpp
 * @since     Nov 7, 2018
 * @author    Patricio A. Rossi (MeduZa)
 * @copyright Copyright © 2018 - 2019 Patricio A. Rossi (MeduZa)
 */

#include "GroovyGameGear.hpp"

#ifndef LEDWIZ32_HPP_
#define LEDWIZ32_HPP_ 1

#define LEDWIZ32_NAME          "Led-Wiz 32"
#define LEDWIZ32_PRODUCT       0x00f0
#define LEDWIZ32_WVALUE        0x0200
#define LEDWIZ32_INTERFACE     0
#define LEDWIZ32_LEDS          32
#define LEDWIZ32_MAX_BOARDS    16

namespace LEDSpicer {
namespace Devices {
namespace GroovyGameGear {

/**
 * LEDSpicer::Devices::GroovyGameGear::LedWiz32
 *
 * Led-Wiz 32 controller class.
 */
class LedWiz32 : public GroovyGameGear {

public:

	LedWiz32(uint8_t boardId, umap<string, string>& options) :
	GroovyGameGear(
		LEDWIZ32_WVALUE,
		LEDWIZ32_INTERFACE,
		LEDWIZ32_LEDS,
		LEDWIZ32_MAX_BOARDS,
		boardId,
		LEDWIZ32_NAME
	) {}

	virtual ~LedWiz32() = default;

	virtual void drawHardwarePinMap();

	void transfer();

	uint16_t getProduct();

	void resetLeds();

protected:

	virtual void afterConnect() {}

	virtual void afterClaimInterface();
};

} /* namespace GroovyGameGear */
} /* namespace Devices */
} /* namespace LEDSpicer */

deviceFactory(LEDSpicer::Devices::GroovyGameGear::LedWiz32)

#endif /* LEDWIZ32_HPP_ */
