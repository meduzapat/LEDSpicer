/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      PacDrive.hpp
 * @since     Sep 19, 2018
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2018 - 2025 Patricio A. Rossi (MeduZa)
 *
 * @copyright LEDSpicer is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * @copyright LEDSpicer is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * @copyright You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "Ultimarc.hpp"
#include "utility/Monochromatic.hpp"

#ifndef PACDRIVE_HPP_
#define PACDRIVE_HPP_ 1

#define PAC_DRIVE_NAME      "Ultimarc Pac Drive Controller"
#define PAC_DRIVE_PRODUCT    0x1500
#define PAC_DRIVE_WVALUE     0x0200
#define PAC_DRIVE_INTERFACE  0
#define PAC_DRIVE_LEDS       16
#define PAC_DRIVE_MAX_BOARDS 4
#define PAC_DRIVE_TRANSFER   0 // All.

namespace LEDSpicer::Devices::Ultimarc {

/**
 * LEDSpicer::PacLED
 */
class PacDrive : public Ultimarc, public Monochromatic {

public:

	PacDrive(umap<string, string>& options) :
	Ultimarc(
		PAC_DRIVE_WVALUE,
		PAC_DRIVE_INTERFACE,
		PAC_DRIVE_LEDS,
		PAC_DRIVE_MAX_BOARDS,
		options,
		PAC_DRIVE_NAME
	),
	Monochromatic(options, PAC_DRIVE_NAME) {}

	virtual ~PacDrive() = default;

	void drawHardwareLedMap() override;

	void transfer() const override;

	uint16_t getProduct() const override;

	const bool isProductBasedId() const override;

protected:

	void afterConnect() override {}
};

deviceFactory(PacDrive)

} /* namespace */

#endif /* PACDRIVE_HPP_ */
