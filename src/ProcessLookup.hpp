/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      ProcessLookup.hpp
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

// To handle daemonization and uid/gid.
#include <unistd.h>

#include <fstream>
#include <iostream>
using std::cout;
using std::endl;

#include <chrono>
using std::chrono::milliseconds;
#include <thread>

// To handle c signals.
#include <csignal>
using std::signal;

#include "utility/XMLHelper.hpp"

// for dirs
#include <sys/types.h>
#include <dirent.h>

#ifndef PROCESSLOOKUP_HPP_
#define PROCESSLOOKUP_HPP_ 1

namespace LEDSpicer {

#define PROC_DIRECTORY    "/proc/"
#define NODE_MAIN_PROCESS "processLookup"
#define NODE_MAP          "map"

#define PARAM_MILLISECONDS "runEvery"
#define PARAM_PROCESS_NAME "processName"
#define PARAM_PROCESS_POS  "position"
#define PARAM_SYSTEM       "system"

#define CMDLINE "/cmdline"

#ifdef MiSTer
#define MISTER_BINARY "MiSTer"
#endif

bool running = true;

string currentConfigFile;

int main(int argc, char **argv);

#endif /* PROCESSLOOKUP_HPP_ */

} /* namespace LEDSpicer */
