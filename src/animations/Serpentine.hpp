/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file		Serpentine.hpp
 * @since		Jun 22, 2018
 * @author		Patricio A. Rossi (MeduZa)
 * @copyright	Copyright © 2018 Patricio A. Rossi (MeduZa)
 */

#include "Animation.hpp"
#include "../utility/Color.hpp"

#ifndef SERPENTINE_HPP_
#define SERPENTINE_HPP_ 1

namespace LEDSpicer {
namespace Animations {

/**
 * LEDSpicer::Serpentine
 */
class Serpentine: public Animation {

public:


	Serpentine(umap<string, string>& parameters, Group& layout) :
		Animation(parameters, layout),
		color(parameters["color"]),
		tailColor(parameters["tailColor"]),
		cDirection(str2direction(parameters["direction"])),
		direction(cDirection),
		tailLength(stoi(parameters["tailLength"])),
		tailEffect(str2effects(parameters["tailEffect"])) {}

	virtual ~Serpentine() {}

	virtual void drawConfig();

protected:

	Color
		color,
		tailColor;

	Directions
		cDirection,
		direction;

	Effects
		tailEffect;

	uint8_t
		tailLength,
		position = 0;

	virtual void calculateFrame();
};

}} /* namespace LEDSpicer */

#endif /* SERPENTINE_HPP_ */
