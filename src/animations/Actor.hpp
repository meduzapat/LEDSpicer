/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Actor.hpp
 * @since     Jun 22, 2018
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2018 - 2019 Patricio A. Rossi (MeduZa)
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
#include "utility/Log.hpp"
#include "utility/Utility.hpp"

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

	enum class Directions : uint8_t {Stop, Forward, Backward, ForwardBouncing, BackwardBouncing};

	enum class Speed      : uint8_t {VeryFast, Fast, Normal, Slow, VerySlow};

	/**
	 * Create a new actor object
	 * @param parameters A list of parameters from the animation file.
	 * @param group the group where the actors will act.
	 * @param requiredParameters A list of mandatory parameters.
	 */
	Actor(umap<string, string>& parameters, Group* const group, const vector<string>& requiredParameters);

	Actor() = delete;

	virtual ~Actor() = default;

	/**
	 * Draws the next frame.
	 * @return true if the cycle ended.
	 */
	bool drawFrame();

	/**
	 * Draws the actor configuration.
	 */
	virtual void drawConfig();

	/**
	 * Reset the animation back to begin.
	 */
	virtual void restart();

	static string direction2str(Directions direction);
	static Directions str2direction(const string& direction);

	static string speed2str(Speed speed);
	static Speed str2speed(const string& speed);

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
	 * Advances the system frame forward.
	 */
	static void advanceFrame();

	/**
	 * @return true if the animation is on the bouncing period.
	 */
	bool isBouncing() const;

	/**
	 * @return true if the animation is a bouncer.
	 */
	bool isBouncer() const;

	/**
	 * @return true if the current frame is the first frame.
	 */
	bool isFirstFrame() const;

	/**
	 * @return true if the current frame is the last frame,
	 */
	bool isLastFrame() const;

	/**
	 * @return true if the direction is forward or forward with bouncing.
	 */
	bool isDirectionForward();

	/**
	 * @return true if the direction is backward or backward with bouncing.
	 */
	bool isDirectionBackward();

protected:

	uint8_t
		/// Internally used by actors to keep track of incremental frames.
		currentActorFrame = 1,
		/// Total actor frames.
		totalActorFrames;

	Directions
		/// Current Direction
		cDirection,
		/// Direction
		direction;

	/// The current speed.
	Speed speed;

	/// How the color information will be draw back.
	Color::Filters filter = Color::Filters::Normal;

	static uint8_t
		/// Hardware current frame.
		currentFrame,
		/// Hardware frames per second.
		FPS;

	/**
	 * Array with a list of affected elements.
	 */
	vector<bool> affectedElements;

	/**
	 * Do the elements calculation.
	 * @return an array with the affected elements.
	 */
	virtual const vector<bool> calculateElements() = 0;

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
	 * Moves the frame to the next one.
	 */
	virtual void advanceActorFrame();

	/**
	 * Will calculate the next element index for a group of element, based on the direction and the current position.
	 * @param [out] currentDirection will be updated with the new direction.
	 * @param element the element index from 1.
	 * @param direction
	 * @param totalElements
	 * @return the next element.
	 */
	static uint8_t calculateNextOf(Directions& currentDirection, uint8_t element, Directions direction, uint8_t totalElements);

	/**
	 * Creates an array of colors from a string of comma separated color names.
	 * @param colors
	 * @return
	 */
	static vector<const Color*> extractColors(string colors);

	/**
	 * Sets the affected elements to the desired stated.
	 * @param value
	 */
	void affectAllElements(bool value = false);

private:

	/// A pointer to the real group of elements.
	Group* const group;
};

// The functions to create and destroy actors.
#define actorFactory(plugin) \
	extern "C" LEDSpicer::Animations::Actor* createActor(umap<string, string>& parameters, LEDSpicer::Devices::Group* const group) { return new plugin(parameters, group); } \
	extern "C" void destroyActor(LEDSpicer::Animations::Actor* instance) { delete instance; }

}} /* namespace LEDSpicer::Animations */

#endif /* ACTOR_HPP_ */
