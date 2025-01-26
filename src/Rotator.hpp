/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Rotator.hpp
 * @since     Aug 7, 2020
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

#include <iostream>
using std::cout;
using std::endl;

#include "restrictors/Restrictor.hpp"
#include "restrictors/UltraStik360.hpp"
#include "restrictors/ServoStik.hpp"
#include "restrictors/GPWiz49.hpp"
#include "restrictors/GPWiz40RotoX.hpp"
#include "restrictors/TOS428.hpp"
#include "utility/XMLHelper.hpp"

#ifndef ROTATOR_HPP_
#define ROTATOR_HPP_ 1

#define RESTRICTOR "restrictor"
#define RESTRICTORS RESTRICTOR "s"
#define PLAYER_MAP "map"

namespace LEDSpicer {

using Restrictors::Restrictor;
using Restrictors::UltraStik360;
using Restrictors::ServoStik;
using Restrictors::GPWiz49;
using Restrictors::GPWiz40RotoX;
using Restrictors::TOS428;

int main(int argc, char **argv);

#endif /* ROTATOR_HPP_ */

} /* namespace LEDSpicer */
