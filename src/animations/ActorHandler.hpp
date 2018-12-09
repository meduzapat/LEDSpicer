/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      ActorHandler.hpp
 * @since     Dec 8, 2018
 * @author    Patricio A. Rossi (MeduZa)
 * @copyright Copyright © 2018 Patricio A. Rossi (MeduZa)
 */

#include "Handler.hpp"
#include "Actor.hpp"

#ifndef ACTORHANDLER_HPP_
#define ACTORHANDLER_HPP_ 1

namespace LEDSpicer {
namespace Animations {

/**
 * LEDSpicer::Animations::ActorHandler
 */
class ActorHandler : public Handler {

public:

	ActorHandler() = default;

	ActorHandler(const string& actorName);

	virtual ~ActorHandler() {}

	Actor* createActor(umap<string, string>& parameters, Group* const group);

	void destroyActor(Actor* actor);

protected:

	Actor*(*createFunction)(umap<string, string>&, Group* const) = nullptr;

	void(*destroyFunction)(Actor*) = nullptr;
};

} /* namespace Animations */
} /* namespace LEDSpicer */

#endif /* ACTORHANDLER_HPP_ */
