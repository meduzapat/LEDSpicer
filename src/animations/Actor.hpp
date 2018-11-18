/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Actor.hpp
 * @since     Jun 22, 2018
 * @author    Patricio A. Rossi (MeduZa)
 * @copyright Copyright © 2018 Patricio A. Rossi (MeduZa)
 */

// For ints.
#include <cstdint>

// To handle dynamic arrays.
#include <vector>
using std::vector;

#include "../devices/Group.hpp"
#include "../utility/Color.hpp"
#include "../utility/Log.hpp"
#include "../utility/Utility.hpp"

#ifndef ANIMATION_HPP_
#define ANIMATION_HPP_ 1

#define DIRECTION_NONE     0
#define DIRECTION_FORWARD  1
#define DIRECTION_BACKWARD 2
#define DIRECTION_BOUNCING 4

#define REQUIRED_PARAM_ACTOR {"type", "group", "speed", "direction", "filter"}

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

	Actor(umap<string, string>& parameters, Group* const group);

	virtual ~Actor() {}

	/**
	 * Draws the next frame.
	 * @return the true if the cycle ended.
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
	 * Creates an array of colors from a string of comma separted color names.
	 * @param colors
	 * @return
	 */
	static vector<const Color*> extractColors(string colors);

private:

	/// A pointer to the real group of elements.
	Group* const group;
};

}} /* namespace LEDSpicer::Animations */

#endif /* ANIMATION_HPP_ */
