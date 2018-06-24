/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file		Serpentine.hpp
 * @since		Jun 22, 2018
 * @author		Patricio A. Rossi (MeduZa)
 * @copyright	Copyright © 2018 Patricio A. Rossi (MeduZa)
 */

#ifndef SERPENTINE_HPP_
#define SERPENTINE_HPP_ 1

#include "Animation.hpp"
#include "../Color.hpp"

namespace LEDSpicer {
namespace Animations {

/**
 * LEDSpicer::Serpentine
 */
class Serpentine: public Animation {

public:


	Serpentine(const umap<string, string>& parameters, Group& layout) :
		Animation(parameters, layout),
		color(parameters['color']),
		tailColor(parameters['tailColor']),
		cDirection(parameters['direction']),
		direction(parameters['direction']),
		tailLength(parameters['tailLength']),
		tailEffect(parameters['tailEffect'])
	{}

	virtual ~Serpentine() {}

protected:

	Color
		color,
		tailColor;
	uint8_t
		cDirection = 0,
		direction  = 0,
		tailLength = 0,
		tailEffect = 0,
		position   = 0;

	virtual void calculateFrame();
};

}} /* namespace LEDSpicer */

#endif /* SERPENTINE_HPP_ */
