/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Actions.hpp
 * @since     May 23, 2019
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2018 - 2024 Patricio A. Rossi (MeduZa)
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

#ifndef ACTIONS_HPP_
#define ACTIONS_HPP_ 1

#include "utility/Speed.hpp"
#include "Reader.hpp"

namespace LEDSpicer::Inputs {

/**
 * LEDSpicer::Inputs::Actions
 *
 * This plugin will map actions to buttons to recreate an interaction with the controls
 */
class Actions: public Reader, public Speed {

public:

	Actions(umap<string, string>& parameters, umap<string, Items*>& inputMaps);

	virtual ~Actions() = default;

	virtual void process();

	virtual void activate();

	virtual void deactivate();

	virtual void drawConfig();

protected:

	uint8_t
		frames,
		cframe = 0;

	/// Keeps the ON/OFF flag.
	bool on = false;

	/// Flag to know if the effect is blink or just stay on.
	bool doBlink = true;

	struct Record {

		Record() = default;

		Record(uint16_t map, bool active, Items* item, Record* next) :
			map(map),
			active(active),
			item(item),
			next(next) {}

		Record(const Record& other) :
			map(other.map),
			active(other.active),
			item(other.item),
			next(other.next) {}

		/// Key Map value.
		uint16_t map = 0;

		/// If true, the item is active.
		bool active = false;
		/// Element to turn on when called.
		Items* item = nullptr;

		/// Next element on the list.
		Record* next = nullptr;
	};

	/// Lookup table to avoid search.
	struct LookupMap {
		uint
			groupIdx = 0,
			mapIdx   = 0;
	};

	/// Keeps a list of key to handle a record.
	vector<vector<Record>> groupsMaps;

	/// Small lookup table to avoid a loop every time we need to seek a value.
	umap<uint16_t, LookupMap> groupMapLookup;

	/// list of 1st elements for groups.
	vector<uint16_t> firstItems;

	/// Elements or Groups blinking.
	umap<uint16_t, Items*> blinkingItems;

	void blink();

};

} /* namespace */

inputFactory(LEDSpicer::Inputs::Actions)

#endif /* ACTIONS_HPP_ */
