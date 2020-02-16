/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      FF00SharedCode.hpp
 * @since     Apr 7, 2019
 * @author    Patricio A. Rossi (MeduZa)
 * @copyright Copyright © 2018 - 2020 Patricio A. Rossi (MeduZa)
 */

#include "Ultimarc.hpp"

#ifndef FF00SHAREDCODE_HPP_
#define FF00SHAREDCODE_HPP_ 1

#define FF00_MSG(byte1, byte2) {byte1, byte2}

namespace LEDSpicer {
namespace Devices {
namespace Ultimarc {

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
		uint8_t  elements,
		uint8_t  maxBoards,
		uint8_t  boardId,
		const string& name
	) : Ultimarc(
		wValue,
		interface,
		elements,
		maxBoards,
		boardId,
		name
	) {}

	virtual ~FF00SharedCode() = default;

	virtual void resetLeds();

	void transfer();

protected:

	virtual void afterConnect() {}
};

} /* namespace Ultimarc */
} /* namespace Devices */
} /* namespace LEDSpicer */

#endif /* NANOPACSHAREDCODE_HPP_ */
