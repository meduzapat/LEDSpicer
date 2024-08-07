/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Blinker.hpp
 * @since     Aug 17, 2019
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

#include "Reader.hpp"
#include "utility/Speed.hpp"

#ifndef BLINKER_HPP_
#define BLINKER_HPP_ 1

#define DEFAULT_BLINKS "5"

namespace LEDSpicer::Inputs {

/**
 * LEDSpicer::Blinker
 */
class Blinker: public Inputs::Reader, public Speed {

public:

	Blinker(umap<string, string>& parameters, umap<string, Items*>& inputMaps) :
		Reader(parameters, inputMaps),
		Speed(parameters.count("speed") ? parameters["speed"] : "Normal"),
		frames(static_cast<uint8_t>(speed) * 3),
		times(Utility::parseNumber(parameters.count("times") ? parameters["times"] : DEFAULT_BLINKS, "Invalid numeric value ")) {}

	virtual ~Blinker() = default;

	void process() override;

	void deactivate() override;

	void drawConfig() const override;

protected:

	uint8_t
		times,
		frames,
		cframe = 0;

	bool on = false;

	struct Times {
		Items* item;
		uint8_t times = 0;
	};

	umap<string, Times> blinkingItems;

	vector<string> itemsToClean;

	void blink();

};
} /* namespace */

#endif /* BLINKER_HPP_ */
