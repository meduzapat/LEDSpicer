/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Impulse.hpp
 * @since     May 23, 2019
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2018 - 2020 Patricio A. Rossi (MeduZa)
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

#ifndef IMPULSE_HPP_
#define IMPULSE_HPP_ 1

#include "Reader.hpp"

namespace LEDSpicer::Inputs {

/**
 * LEDSpicer::Inputs::Impulse
 */
class Impulse: public Reader {

public:

	using Reader::Reader;

	virtual ~Impulse() = default;

	virtual void drawConfig();

	virtual void process();
};

} /* namespace */

inputFactory(LEDSpicer::Inputs::Impulse)

#endif /* IMPULSE_HPP_ */
