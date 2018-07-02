/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file		Mist.hpp
 * @ingroup
 * @since		Jun 25, 2018
 * @author		Patricio A. Rossi (MeduZa)
 * @copyright	Copyright © 2018 Patricio A. Rossi (MeduZa)
 */

#ifndef MIST_HPP_
#define MIST_HPP_ 1

#include "Actor.hpp"

namespace LEDSpicer {
namespace Animations {

/**
 * LEDSpicer::Animations::Mist
 */
class Mist: public Actor {

public:

	Mist(umap<string, string>& parameters, Group& layout) :
		Actor(parameters, layout),
		color1(parameters["color1"]),
		color2(parameters["color2"]) {}

	virtual ~Mist() {}

	virtual void drawConfig();

protected:

	Color
		color1,
		color2;

	virtual void canAdvaceFrame();
};

} /* namespace Animations */
} /* namespace LEDSpicer */

#endif /* MIST_HPP_ */
