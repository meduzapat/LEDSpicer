/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file		Serpentine.hpp
 * @since		Jun 22, 2018
 * @author		Patricio A. Rossi (MeduZa)
 * @copyright	Copyright © 2018 Patricio A. Rossi (MeduZa)
 */

#include "../utility/Color.hpp"
#include "Actor.hpp"

#ifndef SERPENTINE_HPP_
#define SERPENTINE_HPP_ 1

#define REQUIRED_PARAM_ACTOR_SERPENTINE {"tailLength", "tailColor", "tailIntensity"}

namespace LEDSpicer {
namespace Animations {

/**
 * LEDSpicer::Serpentine
 */
class Serpentine: public Actor {

public:

	Serpentine(umap<string, string>& parameters, Group* const layout);

	virtual ~Serpentine() {}

	virtual void drawConfig();

protected:

	uint8_t
		/// Current change frame factor.
		changeFrame = 1,
		/// Total number of frames to actually move to the next actor frame.
		changeFrameTotal;

	Color
		color,
		tailColor;

	struct TailData {
		uint8_t percent;
		uint8_t position;
	};
	vector<TailData> tailData;

	void calculateTailPosition();

	virtual void calculateElements();

	virtual void advanceActorFrame();
};

}} /* namespace LEDSpicer */

#endif /* SERPENTINE_HPP_ */
