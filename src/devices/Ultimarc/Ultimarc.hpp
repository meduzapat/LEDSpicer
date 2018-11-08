/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Ultimarc.hpp
 * @since     Sep 27, 2018
 * @author    Patricio A. Rossi (MeduZa)
 * @copyright Copyright © 2018 Patricio A. Rossi (MeduZa)
 */

#include "../Device.hpp"

#ifndef ULTIMARC_HPP_
#define ULTIMARC_HPP_ 1

/// Ultimarc USB vendor code.
#define ULTIMARC_VENDOR       0xD209
/// The request type field for the setup packet.
#define ULTIMARC_REQUEST_TYPE 0x21
/// The request field for the setup packet.
#define ULTIMARC_REQUEST      9

namespace LEDSpicer {
namespace Devices {
namespace Ultimarc {

/**
 * Ultimarc family data and definitions.
 */
class Ultimarc : public Device {

	using Device::Device;

	uint16_t getVendor() {
		return ULTIMARC_VENDOR;
	}

};

}}}

#endif /* ULTIMARC_HPP_ */
