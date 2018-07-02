/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file		Actor.hpp
 * @since		Jun 22, 2018
 * @author		Patricio A. Rossi (MeduZa)
 * @copyright	Copyright © 2018 Patricio A. Rossi (MeduZa)
 */

// To handle unordered map.
#include <unordered_map>
#ifndef umap
	#define umap std::unordered_map
#endif

// For ints.
#include <cstdint>

// To handle dynamic arrays.
#include <vector>
using std::vector;

#include "../devices/Group.hpp"
#include "../utility/Color.hpp"
#include "../utility/Error.hpp"
#include "../utility/Utility.hpp"

#define DIRECTION_NONE 0
#define DIRECTION_FORWARD 1
#define DIRECTION_BACKWARD 2
#define DIRECTION_BOUNCING 4

#ifndef ANIMATION_HPP_
#define ANIMATION_HPP_ 1

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
	 * @return the current actor frame.
	 */
	uint8_t drawFrame();

	/**
	 * Returns the total number of frames this Actor needs.
	 * @return
	 */
	const uint8_t getTotalFrames() const;

	/**
	 * Draws the actor configuration.
	 */
	virtual void drawConfig();

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

protected:

	uint8_t
		changeFrame = 1,
		changeFrameTotal,
		/// Current actor frame.
		currentActorFrame = 1,
		/// Total actor frames.
		totalActorFrames;

	Directions
		/// Current Direction
		cDirection,
		/// Direction
		direction;

	/// How the color information will be draw back.
	Color::Filters filter = Color::Filters::Normal;

	static uint8_t
		currentFrame,
		FPS;

	virtual void calculateElements() = 0;

	uint8_t getNumberOfElements() const;

	void changeElementColor(uint8_t index, Color color, Color::Filters filter, uint8_t percent = 50);

	void changeElementsColor(Color color, Color::Filters filter, uint8_t percent = 50);

	/**
	 * Calculates and advances the frame.
	 * @return true if the frame was advanced.
	 */
	virtual bool canAdvaceFrame();

	void advanceActorFrame();

	uint8_t calculateNextFrame(Directions& cDirection, uint8_t element);

private:

	/// A pointer to the real group of elements.
	Group* const group;
};

}} /* namespace LEDSpicer::Animations */

#endif /* ANIMATION_HPP_ */
