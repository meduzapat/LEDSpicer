/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      NanoLed.hpp
 * @since     Mar 11, 2019
 * @author    Patricio A. Rossi (MeduZa)
 * @copyright Copyright © 2018 - 2024 Patricio A. Rossi (MeduZa)
 */

#include "FF00SharedCode.hpp"

#ifndef NANOLED_HPP_
#define NANOLED_HPP_ 1

#define NANO_LED_NAME       "Ultimarc NanoLed"
#define NANO_LED_PRODUCT    0x1481
#define NANO_LED_WVALUE     0x0200
#define NANO_LED_INTERFACE  1
#define NANO_LED_LEDS       60
#define NANO_LED_MAX_BOARDS 4

namespace LEDSpicer::Devices::Ultimarc {

/**
 * LEDSpicer::Devices::Ultimarc::NanoLed
 *
 * This library contains the code to control the Ultimarc Nanoled board.
 */
class NanoLed: public FF00SharedCode {

public:

	NanoLed(umap<string, string>& options) :
	FF00SharedCode(
		NANO_LED_WVALUE,
		0, // to be defined.
		NANO_LED_LEDS,
		NANO_LED_MAX_BOARDS,
		options,
		NANO_LED_NAME
	) {}

	virtual ~NanoLed() = default;

	void drawHardwareLedMap() override;

	uint16_t getProduct() const override;

};

deviceFactory(NanoLed)

} /* namespace */

#endif /* NANOLED_HPP_ */
