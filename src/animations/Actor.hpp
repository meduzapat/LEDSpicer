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

	virtual ~Actor();

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
	 * @return Returns the amount of time an actor will be active or 0 if never finishes
	 */
	virtual float getRunTime() const;

	/**
	 * @return a reference to the hardware internal LEDs for the group.
	 */
	const vector<uint8_t*>& getLeds() const;

protected:

	inline static uint8_t
		/// Hardware frames per second.
		FPS   = 0,
		/// Current frame (starting from 1)
		frame = 0;

	/// Keep truck of actors
	const uint32_t actorNumber;

	/// Actor counter.
	inline static uint32_t actorCount = 0;

	/// Color filter applied to elements.
	Color::Filters filter = Color::Filters::Normal;

	const float
		/// Number of seconds to wait until start processing this actor.
		secondsToStart   = 0,
		/// Number of seconds to wait to stop this actor.
		secondsToEnd     = 0,
		/// Number of seconds to wait after endTime before restarting.
		secondsToRestart = 0;

	Time
		/// Start time clock for this actor.
		* startTime   = nullptr,
		/// End time clock for this actor.
		* endTime     = nullptr,
		/// Restart delay clock, armed after endTime expires.
		* restartTime = nullptr;

	/// If this actor will repeat, default -1 to repeat forever.
	const int16_t repeat;
	/// Times repeated.
	uint16_t repeated = 0;

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

private:

	/// Array with a list of affected elements.
	vector<bool> affectedElements;

	/// A pointer to the real group of elements.
	Group* const group;

};

using ActorPtrsUmap = unordered_map<string, vector<Actor*>>;

} // namespace
