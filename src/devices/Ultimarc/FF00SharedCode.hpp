/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      FF00SharedCode.hpp
 * @since     Apr 7, 2019
 * @author    Patricio A. Rossi (MeduZa)
 * @copyright Copyright © 2018 - 2024 Patricio A. Rossi (MeduZa)
 */

#include "Ultimarc.hpp"

#ifndef FF00SHAREDCODE_HPP_
#define FF00SHAREDCODE_HPP_ 1

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
		uint8_t  pins,
		uint8_t  maxBoards,
		umap<string, string>& options,
		const string& name
	) : Ultimarc(
		wValue,
		interface,
		pins,
		maxBoards,
		options,
		name
	) {}

	virtual ~FF00SharedCode() = default;

	virtual void resetLeds();

	void transfer() const;

protected:

	virtual void afterConnect() {}
};

} /* namespace */

#endif /* NANOPACSHAREDCODE_HPP_ */
