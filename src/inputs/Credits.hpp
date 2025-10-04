/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Credits.hpp
 * @since     Apr 15, 2024
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

#include "utilities/Speed.hpp"
#include "Reader.hpp"

#pragma once

#define DEFAULT_COINS "1"
#define DEFAULT_MODE  "Multi"

namespace LEDSpicer::Inputs {

/**
 * LEDSpicer::Inputs::Credits
 *
 * Credit -> start interaction input plugin.
 */
class Credits: public Reader, public Speed {

public:

	enum class Modes : uint8_t {
		/**
		 * Credits will blink the next available START.
		 * if alwaysOn is not set, Coin will go off.
		 * More Credits will turn more STARTs.
		 * If an active START is pressed, COIN will start blinking (if once is not set)
		 */
		Single,
		/**
		 * Credits will blink the STARTs one by one, until all are blinking.
		 * if alwaysOn is not set, Coin will go off.
		 * If an active START is pressed, COIN will start blinking (if once is not set)
		 */
		Multi
	};

	Credits(StringUMap& parameters, ItemPtrUMap& inputMaps);

	virtual ~Credits() = default;

	void process() override;

	void activate() override;

	void deactivate() override;

	void drawConfig() const override;

protected:

	struct Record {

		Record() = default;

		Record(string map, bool active, Items* item) :
			map(map),
			active(active),
			item(item) {}

		Record(const Record& other) :
			map(other.map),
			active(other.active),
			item(other.item) {}

		/// trigger map value.
		string map;

		/// If true, the item is active.
		bool active = false;

		/// Element or Group to turn on when called.
		Items* item = nullptr;
	};

	/// Lookup table to avoid search.
	struct LookupMap {
		uint
			groupIdx = 0,
			mapIdx   = 0;
	};

	uint8_t
		frames,
		cframe = 0,
		coinsPerCredit;

	Modes mode = Modes::Multi;

	bool
		once     = false,
		alwaysOn = false,
		/// Keeps the ON/OFF flag.
		on       = false;

	/// Keep the number of coins inserted by groupId.
	vector<uint8_t> coinCount;

	/// Keeps a list of key to handle a record.
	vector<vector<Record>> groupsMaps;

	/// Small lookup table by trigger, to avoid a loop every time we need to seek an item.
	unordered_map<string, LookupMap> groupMapLookup;

	/// Elements or Groups blinking.
	ItemPtrUMap blinkingItems;

	void blink();

};
} // namespace
