/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file		Fill.cpp
 * @ingroup
 * @since		Jul 21, 2018
 * @author		Patricio A. Rossi (MeduZa)
 * @copyright	Copyright © 2018 Patricio A. Rossi (MeduZa)
 */

#include "Fill.hpp"

using namespace LEDSpicer::Animations;

Fill::Fill(umap<string, string>& parameters, Group* const layout) :
	Actor(parameters, layout)
{
	totalActorFrames = getNumberOfElements();
	if (parameters["colors"].empty()) {
		colors.reserve(Color::getNames().size());
		for (auto& c : Color::getNames())
			colors.push_back(&Color::getColor(c));
	}
	else {
		for (auto& s : Utility::explode(parameters["colors"], ','))
			colors.push_back(&Color::getColor(s));
	}
}

void Fill::calculateElements() {
	uint8_t colorIdx = 0;
	for (uint8_t c = 0; c < totalActorFrames; ++c) {
		changeElementColor(c, *colors[colorIdx], filter);
		++colorIdx;
		if (colorIdx == colors.size())
			colorIdx = 0;
	}
}

