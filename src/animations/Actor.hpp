/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Actor.hpp
 * @since     Jun 22, 2018
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2018 - 2026 Patricio A. Rossi (MeduZa)
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

#include "devices/Group.hpp"
#include "utilities/Utility.hpp"
#include "utilities/Time.hpp"
#include "utilities/Log.hpp"

#pragma once

/// Required parameters for Actor constructor.
#define REQUIRED_PARAM_ACTOR {"type", "group", "filter"}

namespace LEDSpicer::Animations {

using LEDSpicer::Devices::Group;
using LEDSpicer::Devices::Element;
using namespace LEDSpicer::Utilities;

/**
 * LEDSpicer::Animation
 */
class Actor {

public:

	/**
	 * Creates a new actor object.
	 *
	 * @param parameters Parameters from the animation file.
	 * @param group The group where the actor will act.
	 * @param requiredParameters Mandatory parameters to validate.
	 * @throws Error If parameters are invalid.
	 */
	Actor(
		StringUMap&           parameters,
		Group* const          group,
		const vector<string>& requiredParameters
	);

	Actor() = delete;

	virtual ~Actor() = default;

	/**
	 * @return The group.
	 */
	Group& getGroup() const;

	/**
	 * Draws actor contents.
	 */
	virtual void draw();

	/**
	 * Draws the actor configuration.
	 */
	virtual void drawConfig() const;

	/**
	 * Reset the actor back to begin.
	 * it activates timers.
	 */
	virtual void restart();

	/**
	 * @return Return true if the actor is running animations and reading input.
	 */
	virtual bool isRunning();

	/**
	 * Sets the system FPS.
	 * @param FPS
	 */
	static void setFPS(uint8_t FPS);

	/**
	 * @return Gets the system FPS.
	 */
	static uint8_t getFPS();

	/**
	 *
	 * @return true If the actor can be handled by time.
	 */
	virtual bool acceptTime() {
		return true;
	}

	/**
	 * @return true If the actor can be handled by cycles.
	 */
	virtual bool acceptCycles() const {
		return false;
	}

	/**
	 * Increments the frame counter, resetting at FPS limit.
	 */
	static void newFrame();

	/**
	 * @return the number of elements on the animation's group.
	 */
	uint16_t getNumberOfElements() const;

	/**
	 * Changes an element color with a new one applying filters.
	 *
	 * @param index the element to change.
	 * @param color the new color, only used by filter Normal, Combine, Difference, Mask.
	 * @param filter
	 * @param percent the amount of effect to apply, only used by Combine.
	 */
	void changeElementColor(uint16_t index, const Color& color, Color::Filters filter, uint8_t percent = 50);

	/**
	 * Changes the colors with a new one applying filters.
	 *
	 * @param color the new color, only used by filter Normal, Combine, Difference and Mask.
	 * @param filter
	 * @param percent the amount of effect to apply, only used by Combine.
	 */
	void changeElementsColor(const Color& color, Color::Filters filter, uint8_t percent = 50);

	/**
	 * Returns the total calculated amount of frames.
	 * @return
	 */
	virtual uint16_t getFullFrames() const = 0;

	/**
	 * @return Returns the amount of time to finish (or 0 if never finishes)
	 */
	virtual float getRunTime() const = 0;

	/**
	 * @return a reference to the hardware internal LEDs for the group.
	 */
	const vector<uint8_t*>& getLeds() const;

protected:

	/// Hardware frames per second.
	static uint8_t FPS;

	/// Current frame (starting from 1)
	static uint8_t frame;

	/// Color filter applied to elements.
	Color::Filters filter = Color::Filters::Normal;

	uint16_t
		/// Number of seconds to wait until start processing this actor.
		secondsToStart = 0,
		/// Number of seconds to wait to stop this actor.
		secondsToEnd   = 0;

	Time
		/// Start time clock for this actor.
		* startTime = nullptr,
		/// End time clock for this actor.
		* endTime = nullptr;

	/**
	 * Do the elements calculation.
	 *
	 * Every Actor will do their magic here.
	 */
	virtual void calculateElements() = 0;

	/**
	 * Marks all elements as affected or unaffected.
	 *
	 * @param value True to mark as affected, false otherwise (default).
	 */
	void affectAllElements(bool value = false);

	/**
	 * Checks if an element was affected in the current frame.
	 *
	 * @param index The element index.
	 * @return true if affected.
	 */
	bool isElementAffected(uint16_t index) const;

	/**
	 * Checks if the actor will repeat.
	 * @return true if the actor will repeat.
	 */
	bool checkRepeats();

private:

	/// Array with a list of affected elements.
	vector<bool> affectedElements;

	/// A pointer to the real group of elements.
	Group* const group;

	/// If this actor will repeat, default 0 to repeat for ever.
	const uint8_t repeat;
	/// Times repeated.
	uint8_t	repeated = 1;
};

using ActorPtrsUmap = unordered_map<string, vector<Actor*>>;

} // namespace

// The functions to create and destroy actors.
#define actorFactory(plugin) \
	extern "C" Actor* createActor(StringUMap& parameters, LEDSpicer::Devices::Group* const group) { return new plugin(parameters, group); } \
	extern "C" void destroyActor(Actor* instance) { delete instance; }
