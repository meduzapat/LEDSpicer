/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      ActorDriven.hpp
 * @since     Oct 2, 2025
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2018 - 2026 Patricio A. Rossi (MeduZa)
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

#include "Transition.hpp"
#include "animations/DirectionActor.hpp"

#pragma once

namespace LEDSpicer::Devices::Transitions {

using LEDSpicer::Animations::DirectionActor;

/**
 * This class will transition using an actor.
 */
class ActorDriven: public Transition {

public:

	/**
	 * ActorDriven constructor.
	 * It takes ownership of the actor pointer.
	 * @param current
	 * @param to
	 * @param actor The actor pointer that will handle this transition.
	 */
	ActorDriven(Profile* current, Actor* actor) :
		Transition(current),
		actor(dynamic_cast<DirectionActor*>(actor)) {}

	ActorDriven(const ActorDriven&) = delete;
	ActorDriven& operator=(const ActorDriven&) = delete;

	virtual ~ActorDriven();

	/**
	 * @see Transition::drawConfig()
	 */
	void drawConfig() const override;

protected:

	/// Pointer to the DirectionActor handling this transition.
	DirectionActor* actor = nullptr;

	/**
	 * @see Transition::reset()
	 */
	void reset() override;

};

} // Namespace
