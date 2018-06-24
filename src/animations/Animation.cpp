/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file		Animation.cpp
 * @ingroup
 * @since		Jun 22, 2018
 * @author		Patricio A. Rossi (MeduZa)
 * @copyright	Copyright © 2018 Patricio A. Rossi (MeduZa)
 */

#include "Animation.hpp"


using namespace LEDSpicer::Animations;

Animation::Animation(const umap<string, string>& parameters, Group& layout) :
	speed(parameters["speed"]),
	filter(parameters["filter"]),
	frame(layout),
	group(layout) {}

Animation::~Animation() {
	// TODO Auto-generated destructor stub
}

void Animation::drawFrame() {
	calculateFrame();
	for (uint8_t c1 = 0; c1 < frame.elements.size(); ++c1) {
		for (uint8_t c2 = 0; c2 < frame.elements[c1].pins.size(); ++c2) {
			switch (filter) {
			case Color::Filter::Normal:
				*group.elements[c1].pins[c2] = frame.elements[c1].pins[c2];
			break;
			case Color::Filter::Combine:
				*group.elements[c1].pins[c2] = Color::transition(frame.elements[c1].pins[c2], *group.elements[c1].pins[c2], 50);
			}
		}
	}
}
