/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      UltraStik360.hpp
 * @since     Jul 10, 2020
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2020 Patricio A. Rossi (MeduZa)
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

#include <fstream>
#include "Restrictor.hpp"
#include "Brands.hpp"

#ifndef RESTRICTORS_ULTRASTIK360_HPP_
#define RESTRICTORS_ULTRASTIK360_HPP_ 1

// NOTE: for old ultrastiks before 2015 product is 0x0501 and interface is 0
#define ULTRASTIK_NAME       "UltraStik360"
#define ULTRASTIK_PRODUCT    0x0511
#define ULTRASTIK_INTERFACE  2
#define ULTRASTIK_WVALUE     0x0200
#define ULTRASTIK_MAX_BOARDS 4
#define ULTRASTIK_DATA_SIZE  4

#define UM_FILES_DIR PACKAGE_DATA_DIR "umaps/"
#define DEFAULT_MAP_BORDERS {30,58,86,114,142,170,198,226}
namespace LEDSpicer {
namespace Restrictors {

/**
 * LEDSpicer::Rotators::UltraStik360
 * Notes:
 * byte0 keep Analog: false off(0x50), true on(0x11) (2015+).
 * byte1 map size: is 9 (can change in the future).
 * byte2 mechanical restrictor: false off(0x10), true on(0x09).
 * byte3-10 borders: each number represents the borders of the cells.
 * byte11-92 map data.
 * byte95 Flash: (pre 2015) false RAM(0xFF), true FLASH(0x00), 2015+ should be 0.
 */
class UltraStik360: public Restrictor {

public:

	UltraStik360(umap<string, string>& options) :
		Restrictor(
			options,
			ULTRASTIK_WVALUE,
			ULTRASTIK_INTERFACE,
			Utility::parseNumber(options["boardId"], "Invalid Board ID"),
			ULTRASTIK_MAX_BOARDS
		),
		handleRestrictor(options.count("hasRestrictor") ? options["hasRestrictor"] == "true" : false),
		handleMouse(options.count("handleMouse") ? options["handleMouse"] == "true" : false) {}

	virtual ~UltraStik360() = default;

	virtual void rotate(Ways way);

	virtual uint16_t getVendor();

	virtual uint16_t getProduct();

	virtual string getName();

protected:

	bool
		handleRestrictor,
		handleMouse;

	static umap<Ways, vector<uint8_t>> umdataCache;

	umap<string, vector<uint8_t>> processUmFile(const string& file);

	string ways2file(Ways ways);

};

} /* namespace Restrictors */
} /* namespace LEDSpicer */

#endif /* RESTRICTORS_ULTRASTIK360_HPP_ */
