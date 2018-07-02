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

#define REQUIRED_PARAM_ACTOR_SERPENTINE {"tailLength", "tailColor"}

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

	Color
		color,
		tailColor;

	Directions tailDirection;

	struct TailData {
		uint8_t percent;
		uint8_t position = 0;
	};
	vector<TailData> tailData;

	void calculateTailPosition();

	virtual void calculateElements();

};

}} /* namespace LEDSpicer */

#endif /* SERPENTINE_HPP_ */
