/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file		TimedActor.cpp
 * @ingroup
 * @since		Sep 16, 2018
 * @author		Patricio A. Rossi (MeduZa)
 * @copyright	Copyright © 2018 Patricio A. Rossi (MeduZa)
 */

#include "TimedActor.hpp"

using namespace LEDSpicer::Animations;

TimedActor::TimedActor(umap<string, string>& parameters, Group* const group) :
	Actor(parameters, group)
{
	changeFrameTotal = static_cast<uint8_t>(speed) + 1;
}

void TimedActor::advanceActorFrame() {

	if (changeFrame >= changeFrameTotal) {
		changeFrame = 1;
		Actor::advanceActorFrame();
		return ;
	}
	changeFrame++;
}
