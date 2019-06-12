/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      ActorHandler.cpp
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

#include "ActorHandler.hpp"

using namespace LEDSpicer::Animations;

ActorHandler::ActorHandler(const string& actorName) :
	Handler(ACTORS_DIR + actorName + ".so"),
	createFunction(reinterpret_cast<Actor*(*)(umap<string, string>&, Group* const)>(dlsym(handler, "createActor"))),
	destroyFunction(reinterpret_cast<void(*)(Actor*)>(dlsym(handler, "destroyActor")))
{
	if (char *errstr = dlerror())
		throw Error("Failed to load actor " + actorName + " " + errstr);
}

Actor* ActorHandler::createActor(umap<string, string>& parameters, Group* const group) {
	return createFunction(parameters, group);
}

void ActorHandler::destroyActor(Actor* actor) {
	destroyFunction(actor);
}
