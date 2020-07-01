/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Emitter.hpp
 * @since     Jul 8, 2018
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

#include <iostream>
using std::cout;
using std::endl;

#include <unistd.h>

#include <cstring>

#include <memory>

#include "Messages.hpp"
#include "utility/XMLHelper.hpp"

#ifndef EMITTER_HPP_
#define EMITTER_HPP_ 1

#define CONFIG_FILE PACKAGE_CONF_DIR "/" PACKAGE ".conf"
#define CONTROLLERS_FILE PACKAGE_DATA_DIR "gameData.xml"
#define CONTROL "C"
#define PLAYERS "ps"
#define TYPE    "t"
#define BUTTONS "b"

/**
 * Main function.
 * Handles command line and executes the program.
 *
 * @param argc
 * @param argv
 * @return exit code.
 */
int main(int argc, char **argv);

vector<string> parseMame(const string& rom);

#endif /* EMITTER_HPP_ */
