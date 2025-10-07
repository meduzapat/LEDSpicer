/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Reader.hpp
 * @since     May 23, 2019
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2018 - 2026 Patricio A. Rossi (MeduZa)
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

#include "Input.hpp"
#include <linux/input.h>
#include <fcntl.h>

#pragma once

#define DEV_INPUT "/dev/input/by-id/"

namespace LEDSpicer::Inputs {

/**
 * LEDSpicer::Inputs::Reader
 *
 * Abstract class with input reader to support user interaction on other plugins.
 */
class Reader: public Input {

public:

	Reader(StringUMap& parameters, ItemPtrUMap& inputMaps);

	virtual ~Reader() = default;

	void activate() override;

	void deactivate() override;

	void drawConfig() const override;

protected:

	struct ListenEventData {
		int     rCode;
		uint8_t index;
	};

	struct ReadData {
		string trigger;
		uint16_t type;
		int value;
	};

	/// Detailed poll of events.
	static vector<ReadData> events;

	/// The first plugin will handle the reads.
	static Input* readController;

	/**
	 * list of input device with their resource and index.
	 */
	static unordered_map<string, ListenEventData> listenEvents;

	/**
	 * Reads all the events.
	 */
	void readAll();

};

} // namespace
