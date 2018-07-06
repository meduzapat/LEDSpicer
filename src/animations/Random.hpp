/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file		Random.hpp
 * @since		Jul 5, 2018
 * @author		Patricio A. Rossi (MeduZa)
 * @copyright	Copyright © 2018 Patricio A. Rossi (MeduZa)
 */

#ifndef RANDOM_HPP_
#define RANDOM_HPP_ 1

#include <cstdlib>

#include "Actor.hpp"

#define REQUIRED_PARAM_ACTOR_RANDOM {"seed", "colors"}

namespace LEDSpicer {
namespace Animations {

/**
 * LEDSpicer::Animations::Random
 */
class Random: public Actor {

public:

	Random(umap<string, string>& parameters, Group* const layout);

	virtual ~Random() {}

protected:

	vector<const Color*>
		colors,
		newColors,
		oldColors;

	static vector<string> colorNames;

	void calculateElements();

	void generateNewColors();

	/**
	 * Every frame is necessary to create a smooth transition.
	 * @return
	 */
	bool canAdvaceFrame();

	/**
	 * Random needs to always go forward.
	 */
	void advanceActorFrame();

	void drawConfig();

};

} /* namespace Animations */
} /* namespace LEDSpicer */

#endif /* RANDOM_HPP_ */