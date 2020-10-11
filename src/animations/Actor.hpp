/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Actor.hpp
 * @since     Jun 22, 2018
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2018 - 2020 Patricio A. Rossi (MeduZa)
 *
 * @copyright LEDSpicer is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * @copyright LEDSpicer is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * @copyright You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

// For ints.
#include <cstdint>

// To handle dynamic arrays.
#include <vector>
using std::vector;

#include "devices/Group.hpp"
#include "utility/Color.hpp"
#include "utility/Utility.hpp"
#include "utility/Time.hpp"

#ifndef ACTOR_HPP_
#define ACTOR_HPP_ 1

#define REQUIRED_PARAM_ACTOR {"type", "group", "filter"}

namespace LEDSpicer {
namespace Animations {

using Devices::Group;
using Devices::Element;

/**
 * LEDSpicer::Animation
 */
class Actor {

public:

	/**
	 * Create a new actor object
	 * @param parameters A list of parameters from the animation file.
	 * @param group the group where the actors will act.
	 * @param requiredParameters A list of mandatory parameters.
	 */
	Actor(
		umap<string, string>& parameters,
		Group* const group,
		const vector<string>& requiredParameters
	);

	Actor() = delete;

	virtual ~Actor() = default;

	/**
	 * Draws actor contents.
	 */
	virtual void draw();

	/**
	 * Draws the actor configuration.
	 */
	virtual void drawConfig();

	/**
	 * Reset the animation back to begin.
	 */
	virtual void restart();

	/**
	 * @return Return true if the actor is running.
	 */
	virtual bool isRunning();

	/**
	 * Sets the system FPS.
	 * @param FPS
	 */
	static void setFPS(uint8_t FPS);

	/**
	 * Sets the system FPS.
	 * @param FPS
	 */
	static uint8_t getFPS();

	/**
	 * If the actor can be handled by time.
	 * @return
	 */
	virtual bool acceptTime() {
		return true;
	}

	/**
	 * If the actor can be handled by cycles.
	 * @return
	 */
	virtual bool acceptCycles() {
		return false;
	}

	/**
	 * Change the start time.
	 * @param seconds
	 */
	void setStartTime(uint16_t seconds);

	/**
	 * Change the end time.
	 * @param seconds
	 */
	void setEndTime(uint16_t seconds);

	/**
	 * Change the start cycles.
	 * @param cycles
	 */
	virtual void setStartCycles(uint8_t cycles) {}

	/**
	 * Change the end cycles.
	 * @param cycles
	 */
	virtual void setEndCycles(uint8_t seconds)  {}

	/**
	 * Function called when a new frame begins.
	 */
	static void newFrame();

protected:

	/// How the color information will be draw back.
	Color::Filters filter = Color::Filters::Normal;

	/// Hardware frames per second.
	static uint8_t FPS;

	static uint8_t frame;

	uint16_t
		secondsToStart = 0,
		secondsToEnd   = 0;

	/// The actor will start after a number of seconds.
	Time* startTime = nullptr;

	/// The actor will end after a number of seconds.
	Time* endTime = nullptr;

	/**
	 * Do the elements calculation.
	 */
	virtual void calculateElements() = 0;

	/**
	 * @return the number of elements on this group.
	 */
	uint8_t getNumberOfElements() const;

	/**
	 * Changes the color with a new one applying filters.
	 * @param index the element to change.
	 * @param color the new color, only used by filter Normal, Combine, Difference, Mask.
	 * @param filter
	 * @param percent the amount of effect to apply, only used by Combine.
	 */
	void changeElementColor(uint8_t index, const Color& color, Color::Filters filter, uint8_t percent = 50);

	/**
	 * Changes the colors with a new one applying filters.
	 * @param color the new color, only used by filter Normal, Combine, Difference and Mask.
	 * @param filter
	 * @param percent the amount of effect to apply, only used by Combine.
	 */
	void changeElementsColor(const Color& color, Color::Filters filter, uint8_t percent = 50);

	/**
	 * Mark all elements to the desired state.
	 * @param value
	 */
	void affectAllElements(bool value = false);

	/**
	 * Returns if an element got affected (dirty/changed) on this frame.
	 * @return
	 */
	bool isElementAffected(uint8_t index);

private:

	/**
	 * Array with a list of affected elements.
	 */
	vector<bool> affectedElements;

	/// A pointer to the real group of elements.
	Group* const group;
};

// The functions to create and destroy actors.
#define actorFactory(plugin) \
	extern "C" LEDSpicer::Animations::Actor* createActor(umap<string, string>& parameters, LEDSpicer::Devices::Group* const group) { return new plugin(parameters, group); } \
	extern "C" void destroyActor(LEDSpicer::Animations::Actor* instance) { delete instance; }

}} /* namespace LEDSpicer::Animations */

#endif /* ACTOR_HPP_ */
