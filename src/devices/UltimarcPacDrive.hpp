/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file		UltimarcPacDrive.hpp
 * @since		Sep 19, 2018
 * @author		Patricio A. Rossi (MeduZa)
 * @copyright	Copyright © 2018 Patricio A. Rossi (MeduZa)
 */

#include "Ultimarc.hpp"

#ifndef PACDRIVE_HPP_
#define PACDRIVE_HPP_ 1

#define PAC_DRIVE "PAC_DRIVE"

#define PAC_DRIVE_PRODUCT       0x1500
//#define PAC_DRIVE_VALUE         0x0203
#define PAC_DRIVE_VALUE         0x0200
#define PAC_DRIVE_INTERFACE     0
#define PAC_DRIVE_LEDS          16

namespace LEDSpicer {
namespace Devices {

/**
 * LEDSpicer::PacLED
 */
class UltimarcPacDrive: public Ultimarc {

public:

	/**
	 * @param boardId
	 */
	UltimarcPacDrive(uint8_t boardId, umap<string, string>& options);

	virtual ~UltimarcPacDrive() {}

	virtual void drawHardwarePinMap();

	void transfer();

protected:

	uint8_t changePoint = 64;

	virtual void afterConnect();
};

}} /* namespace LEDSpicer */

#endif /* PACDRIVE_HPP_ */
