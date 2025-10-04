/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      FF00SharedCode.hpp
 * @since     Apr 7, 2019
 * @author    Patricio A. Rossi (MeduZa)
 * @copyright Copyright © 2018 - 2025 Patricio A. Rossi (MeduZa)
 */

#include "Ultimarc.hpp"

#pragma once

#define FF00_MSG(byte1, byte2) {byte1, byte2}
#define FF00_TRANSFER 2 // Batches.

namespace LEDSpicer::Devices::Ultimarc {

/**
 * LEDSpicer::Animations::FF00SharedCode
 *
 * Class with shared code for devices that uses the ff00 transfer call.
 */
class FF00SharedCode : public Ultimarc {

public:

	FF00SharedCode(
		uint16_t wValue,
		uint8_t  interface,
		uint16_t  leds,
		uint8_t  maxBoards,
		StringUMap& options,
		const string& name
	) : Ultimarc(
		wValue,
		interface,
		leds,
		maxBoards,
		options,
		name
	) {}

	virtual ~FF00SharedCode() = default;

	void resetLeds() override;

	void transfer() const override;

protected:

	void afterConnect() override {}
};

} // namespace
