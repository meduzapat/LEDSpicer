/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      ActorHandler.cpp
 * @since     Dec 8, 2018
 * @author    Patricio A. Rossi (MeduZa)
 * @copyright Copyright © 2018 Patricio A. Rossi (MeduZa)
 */

#include "ActorHandler.hpp"

using namespace LEDSpicer::Animations;

ActorHandler::ActorHandler(const string& actorName) :
	Handler(ACTORS_DIR + actorName + ".so"),
	createFunction(reinterpret_cast<Actor*(*)(umap<string, string>&, Group* const)>(dlsym(handler, "createActor"))),
	destroyFunction(reinterpret_cast<void(*)(Actor*)>(dlsym(handler, "destroyActor")))
{
	if (not handler or not createFunction or not destroyFunction)
		throw Error("Failed to load actor " + actorName);
}

Actor* ActorHandler::createActor(umap<string, string>& parameters, Group* const group) {
	return createFunction(parameters, group);
}

void ActorHandler::destroyActor(Actor* actor) {
	destroyFunction(actor);
}
