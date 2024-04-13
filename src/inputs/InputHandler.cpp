/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      InputHandler.cpp
 * @since     May 7, 2019
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

#include "InputHandler.hpp"

using namespace LEDSpicer::Inputs;

InputHandler::InputHandler(const string& inputName) :
	Handler(INPUTS_DIR + inputName + ".so"),
	createFunction(reinterpret_cast<Input*(*)(umap<string, string>&, umap<string, Items*>&)>(dlsym(handler, "createInput"))),
	destroyFunction(reinterpret_cast<void(*)(Input*)>(dlsym(handler, "destroyInput")))
{
	if (char *errstr = dlerror())
		throw Error("Failed to load input " + inputName + " " + errstr);
}

InputHandler::~InputHandler() {
	for (Input* i : instances) {
#ifdef DEVELOP
		LogDebug("Input instance deleted");
#endif
		destroyFunction(i);
	}
}

Input* InputHandler::createInput(umap<string, string>& parameters, umap<string, Items*>& maps) {

	Input* instance = createFunction(parameters, maps);
	instances.push_back(instance);
	return instance;
}
