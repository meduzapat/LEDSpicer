/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Filler.hpp
 * @since     Oct 26, 2018
 * @author    Patricio A. Rossi (MeduZa)
 * @copyright Copyright © 2018 - 2019 Patricio A. Rossi (MeduZa)
 */

#include "TimedActor.hpp"

#ifndef FILLER_HPP_
#define FILLER_HPP_ 1

#define REQUIRED_PARAM_ACTOR_FILLER {"speed", "direction", "mode", "color"}

namespace LEDSpicer {
namespace Animations {

/**
 * LEDSpicer::Animations::Fill
 */
class Filler: public TimedActor {

public:

	enum class Modes : uint8_t {Linear, Random, LinearSimple, RandomSimple};

	Filler(umap<string, string>& parameters, Group* const group);

	virtual ~Filler() = default;

	virtual void advanceActorFrame();

	void drawConfig();

	string mode2str(Modes mode);

	Modes str2mode(const string& mode);

protected:

	virtual const vector<bool> calculateElements();

private:

	/// Stores the color.
	Color color;

	/// Stores the mode.
	Modes mode;

	/// Keeps track if the process is filling the group or cleaning.
	bool filling = true;

	/**
	 * Fills the group in a linear way.
	 * @return The elements that changed.
	 */
	void fillElementsLinear(uint8_t begin, uint8_t end);

	/**
	 * Fills the group in a random way.
	 * @param val
	 * @return The elements that changed.
	 */
	void fillElementsRandom(bool val);

	/**
	 * Draw the random values.
	 */
	void drawRandom();

};

} /* namespace Animations */
} /* namespace LEDSpicer */

actorFactory(LEDSpicer::Animations::Filler)

#endif /* FILLER_HPP_ */
