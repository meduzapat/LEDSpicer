/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      GroovyGameGear.hpp
 * @since     Nov 7, 2018
 * @author    Patricio A. Rossi (MeduZa)
 * @copyright Copyright © 2018 - 2019 Patricio A. Rossi (MeduZa)
 */

#include "devices/Device.hpp"

#ifndef GGG_HPP_
#define GGG_HPP_ 1

/// GGG USB vendor code.
#define GGG_VENDOR 0xFAFA

namespace LEDSpicer {
namespace Devices {
namespace GroovyGameGear {

/**
 * LEDSpicer::Devices::GroovyGameGear
 * GGG family data and definitions.
 */
class GroovyGameGear: public Device {

public:

	using Device::Device;

	uint16_t getVendor() {
		return GGG_VENDOR;
	}

};

} /* namespace GroovyGameGear */
} /* namespace Devices */
} /* namespace LEDSpicer */

#endif /* GGG_HPP_ */
