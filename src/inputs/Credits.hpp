/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Credits.hpp
 * @since     Apr 15, 2024
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

#include "utility/Speed.hpp"
#include "Actions.hpp"

#ifndef CREDITS_HPP_
#define CREDITS_HPP_ 1

namespace LEDSpicer::Inputs {

/**
 * LEDSpicer::Inputs::Credits
 *
 * Credit -> start interaction input plugin.
 */
class Credits: public Actions {

public:

	enum class Modes : uint8_t {
		/// Will jump from credit to start and start over.
		Cyclic,
		/// Will jump from credit to start but credit will always be blinking.
		AlwaysOn,
		/// Will jump from credit to start and stop any blinking.
		Once,
		/// Will jump from credit to start, credit will be OFF, but if you press COIN again Credit and next Credit will blink again.
		TwinSpark
	};

	Credits(umap<string, string>& parameters, umap<string, Items*>& inputMaps);

	virtual ~Credits() = default;

	void process() override;

	void activate() override;

	void deactivate() override;

	void drawConfig() override;

protected:

	struct Record {

		Record() = default;

		Record(string coinMap, bool active, Items* item, Record* next) :
			coinMap(coinMap),
			active(active),
			item(item),
			next(next) {}

		Record(const Record& other) :
			map(other.map),
			active(other.active),
			item(other.item),
			next(other.next) {}

		/// trigger map value for coin.
		string coinMap = 0;

		/// If true, the item is active.
		bool active = false;

		/// Element or Group to turn on when called.
		Items* item = nullptr;

		/// Next element or Group on the list.
		Record* next = nullptr;
	};

	Modes mode;

	uint8_t coinsPerCredit;

};

} /* namespace */

#endif /* CREDITS_HPP_ */
