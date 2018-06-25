/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file		Pulse.hpp
 * @since		Jun 24, 2018
 * @author		Patricio A. Rossi (MeduZa)
 * @copyright	Copyright © 2018 Patricio A. Rossi (MeduZa)
 */

#ifndef PULSE_HPP_
#define PULSE_HPP_ 1

#include "Animation.hpp"

namespace LEDSpicer {
namespace Animations {

/**
 * LEDSpicer::Animations::Pulse
 */
class Pulse : public Animation {

public:

	Pulse(umap<string, string>& parameters, Group& layout) :
		Animation(parameters, layout),
		color(parameters["color"]) {}

	virtual ~Pulse() {}

	void drawConfig();

protected:

	Color color;

	virtual void calculateFrame();

};

} /* namespace Animations */
} /* namespace LEDSpicer */

#endif /* PULSE_HPP_ */
