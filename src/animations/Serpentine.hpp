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

	Serpentine(umap<string, string>& parameters, Group& layout) :
		Actor(parameters, layout),
		color(parameters["color"]),
		tailColor(parameters["tailColor"]),
		cDirection(str2direction(parameters["direction"])),
		direction(cDirection),
		tailLength(stoi(parameters["tailLength"])),
		tailweight(100 / (tailLength + 1)) {}

	virtual ~Serpentine() {}

	virtual void drawConfig();

protected:

	Color
		color,
		tailColor;

	uint8_t
		tailLength,
		tailweight;

	virtual void calculateFrame();


};

}} /* namespace LEDSpicer */

#endif /* SERPENTINE_HPP_ */
