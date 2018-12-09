/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      PacDrive.hpp
 * @since     Sep 19, 2018
 * @author    Patricio A. Rossi (MeduZa)
 * @copyright Copyright © 2018 Patricio A. Rossi (MeduZa)
 */

#include "Ultimarc.hpp"

#ifndef PACDRIVE_HPP_
#define PACDRIVE_HPP_ 1

#define PAC_DRIVE "PAC_DRIVE"

#define PAC_DRIVE_NAME      "Ultimarc Pac Drive Controller"
#define PAC_DRIVE_PRODUCT    0x1500
#define PAC_DRIVE_WVALUE     0x0200
#define PAC_DRIVE_INTERFACE  0
#define PAC_DRIVE_LEDS       16
#define PAC_DRIVE_MAX_BOARDS 4

namespace LEDSpicer {
namespace Devices {
namespace Ultimarc {

/**
 * LEDSpicer::PacLED
 */
class PacDrive : public Ultimarc {

public:

	PacDrive(uint8_t boardId, umap<string, string>& options);

	virtual ~PacDrive() {}

	virtual void drawHardwarePinMap();

	void transfer();

	uint16_t getProduct();

protected:

	uint8_t changePoint = 64;

	virtual void afterConnect() {}
};

}}} /* namespace LEDSpicer */

deviceFactory(LEDSpicer::Devices::Ultimarc::PacDrive)

#endif /* PACDRIVE_HPP_ */
