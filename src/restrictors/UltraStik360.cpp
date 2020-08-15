/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      UltraStik360.cpp
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

#include "UltraStik360.hpp"

using namespace LEDSpicer::Restrictors;

umap<UltraStik360::Ways, vector<uint8_t>> UltraStik360::umdataCache;

void UltraStik360::rotate(Ways way) {

	LogDebug("Rotating " + getName() + " to " + ways2str(way));

	vector<uint8_t> data(96, 0);
	uint8_t c;

	if (umdataCache.count(way)) {
		LogDebug("Reusing data for " + ways2file(way));
		data = umdataCache[way];
	}
	else {
		umap<string, vector<uint8_t>> umdata = processUmFile(UM_FILES_DIR + ways2file(way) + ".um");

		if (umdata.empty())
			return;

		data[0] = 0x50;
		data[1] = 9;
		data[2] = handleRestrictor ? 0x09 : 0x10;

		// Process map borders.
		if (not umdata.count("MapBorderLocations"))
			umdata["MapBorderLocations"] = DEFAULT_MAP_BORDERS;
		if (umdata["MapBorderLocations"].size() != 8)
			throw Error("Invalid number of elements for MapBorderLocations");
		c = 3;
		for (uint8_t b : umdata["MapBorderLocations"])
			data[c++] = b;

		// Process Rows.
		for (uint8_t r = 1; r < 10; ++r) {
			string mapname = "MapRow" + to_string(r);
			if (not umdata.count(mapname))
				throw Error("Missing row " + mapname);
			vector<uint8_t> t = umdata[mapname];
			if (t.size() != 9)
				throw Error("Invalid number of elements for " + mapname);
			for (uint8_t b : t)
				data[c++] = b;
		}

		umdataCache.emplace(way, data);
	}

	// Send data.
	vector<uint8_t> dataT;
	for (uint8_t d: data) {
		dataT.push_back(d);
		if (dataT.size() == ULTRASTIK_DATA_SIZE) {
			transferToUSB(dataT);
			dataT.clear();
		}
	}
}

uint16_t UltraStik360::getVendor() {
	return ULTIMARC_VENDOR;
}

uint16_t UltraStik360::getProduct() {
	return (ULTRASTIK_PRODUCT + getId() - 1);
}

string UltraStik360::getName() {
	return string(ULTRASTIK_NAME) + " " + to_string(getId());
}

umap<string, vector<uint8_t>> UltraStik360::processUmFile(const string& file) {

	LogDebug(getName() + " Reading UM file " + file);
	umap<string, vector<uint8_t>> result;

	// Read file.
	std::ifstream infile(file, std::ios::in | std::ios::binary);
	if (not infile.is_open()) {
		LogError("Failed to read " + file);
		return result;
	}
	string content((std::istreambuf_iterator<char>(infile)), (std::istreambuf_iterator<char>()));
	infile.close();

	// Process
	for (string& row : Utility::explode(content, '\n')) {
		if (row.empty())
			continue;
		vector<string> pair = Utility::explode(row, '=');
		if (pair.size() != 2)
			continue;
		vector<uint8_t> r;
		if (pair[0] == "MapBorderLocations") {
			for (string& val : Utility::explode(pair[1], ','))
				r.push_back(Utility::parseNumber(val, "Invalid map location number"));
		}
		else if (pair[0].find("MapRow") != string::npos) {
			for (string& val : Utility::explode(pair[1], ',')) {
				if (val == "-") {
					r.push_back(0x00);
					continue;
				}
				if (val == "C") {
					r.push_back(0x01);
					continue;
				}
				if (val == "N") {
					r.push_back(0x02);
					continue;
				}
				if (val == "NE") {
					r.push_back(0x03);
					continue;
				}
				if (val == "E") {
					r.push_back(0x04);
					continue;
				}
				if (val == "SE") {
					r.push_back(0x05);
					continue;
				}
				if (val == "S") {
					r.push_back(0x06);
					continue;
				}
				if (val == "SW") {
					r.push_back(0x07);
					continue;
				}
				if (val == "W") {
					r.push_back(0x08);
					continue;
				}
				if (val == "NW") {
					r.push_back(0x09);
					continue;
				}
				if (val == "*") {
					r.push_back(0x0A);
					continue;
				}
			}
		}
		else {
			continue;
		}
		result.emplace(pair[0], std::move(r));
	}
	return result;
}

string UltraStik360::ways2file(Ways ways) {
	switch (ways) {
	case Ways::w2:
		return "2Way";
	case Ways::w2v:
		return "2WayV";
	case Ways::w4:
		return "4Way";
	case Ways::w4x:
		return "4WayX";
	case Ways::w8:
		return "8Way";
	case Ways::analog:
    case Ways::w49:
    case Ways::w16:
		return "Analog";
	case Ways::mouse:
		if (handleMouse)
			return "Mouse";
	}
	return "";
}
