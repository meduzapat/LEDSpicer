/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      InputSeeker.hpp
 * @since     Jun 22, 2019
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

#include "config.h"
#include <termios.h>
#include <string>
using std::string;
// To handle IO stream.
#include <iostream>
using std::cout;
using std::endl;
using std::cerr;
#include <dirent.h>
#include <linux/input.h>
#include <fcntl.h>
#include <unistd.h>

// To handle unordered map.
#include <unordered_map>
#ifndef umap
	#define umap std::unordered_map
#endif

#ifndef INPUTSEEKER_HPP_
#define INPUTSEEKER_HPP_ 1

#define DEV_INPUT "/dev/input/by-id/"

namespace LEDSpicer {

/**
 * list of input device and their resource.
 */
umap<string, int> listenEvents;

int main(int argc, char **argv);

}

#endif /* INPUTSEEKER_HPP_ */
