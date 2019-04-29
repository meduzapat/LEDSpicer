/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Handler.cpp
 * @since     Dec 8, 2018
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2018 - 2019 Patricio A. Rossi (MeduZa)
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

#include "Handler.hpp"

using namespace LEDSpicer;

Handler::Handler(const string& filename) :handler(dlopen(filename.c_str(), RTLD_LAZY)) {
	LogDebug("Loading library " + filename);
	if (char *errstr = dlerror())
		throw Error("Failed to load library " + filename + " " + errstr);
}

Handler::~Handler() {
	if (not handler)
		return;
	dlclose(handler);
	handler = nullptr;
}
