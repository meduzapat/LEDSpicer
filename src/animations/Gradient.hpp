/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Gradient.hpp
 * @since     Jul 3, 2018
 * @author    Patricio A. Rossi (MeduZa)
 * @copyright Copyright © 2018 Patricio A. Rossi (MeduZa)
 */

#include "TimedActor.hpp"

#ifndef GRADIENT_HPP_
#define GRADIENT_HPP_ 1

#define REQUIRED_PARAM_ACTOR_GRADIENT {"colors", "mode"}

namespace LEDSpicer {
namespace Animations {

/**
 * LEDSpicer::Animations::Rainbow
 */
class Gradient: public TimedActor {

public:

	enum class Modes : uint8_t {All, Sequential, Cyclic};

	Gradient(umap<string, string>& parameters, Group* const layout);

	virtual ~Gradient() {}

	void drawConfig();

	static Modes str2mode(const string& mode);

	static string mode2str(Modes mode);

protected:

	vector<const Color*> colors;

	Directions colorDirection;

	Modes mode;

	float
		/// the calculated increment
		increment = 0;
	uint8_t
		/// The color for the 1st element.
		currentColor,
		/// Keeps the transition percent.
		currentPercent = 0;

	void calculateElements();

private:

	void calculateSingle();

	void calculateSequential();

	void calculateMultiple();

};

} /* namespace Animations */
} /* namespace LEDSpicer */

#endif /* GRADIENT_HPP_ */
