/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      LedWiz.hpp
 * @since     Nov 7, 2018
 * @author    Patricio A. Rossi (MeduZa)
 * @copyright Copyright © 2018 Patricio A. Rossi (MeduZa)
 */

#include "../Device.hpp"

#ifndef LEDWIZ_HPP_
#define LEDWIZ_HPP_ 1

/// Led-Wiz USB vendor code.
#define LEDWIZ_VENDOR       0xFAFA
/// The request type field for the setup packet.
#define LEDWIZ_REQUEST_TYPE 0x21
/// The request field for the setup packet.
#define LEDWIZ_REQUEST      0x09

namespace LEDSpicer {
namespace Devices {
namespace LedWiz {

/**
 * LEDSpicer::Devices::LedWiz
 * LedWiz family data and definitions.
 */
class LedWiz: public Device {

public:

	using Device::Device;

	uint16_t getVendor() {
		return LEDWIZ_VENDOR;
	}

};

} /* namespace LedWiz */
} /* namespace Devices */
} /* namespace LEDSpicer */

#endif /* LEDWIZ_HPP_ */
