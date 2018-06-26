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

	enum Directions : uint8_t {Stop, Forward, Backward, ForwardBouncing, BackwardBouncing};

	enum Effects    : uint8_t {None, SlowFade, Fade, FastFade};

	Actor(umap<string, string>& parameters, Group& layout);

	virtual ~Actor() {}

	void drawFrame();

	virtual void drawConfig();

	static string direction2str(Directions direction);
	static Directions str2direction(const string& direction);

	static string effects2str(Effects effect);
	static Effects str2effects(const string& effect);

protected:

	uint8_t speed = 0;

	/// Current frame.
	uint8_t frame = 0;

	Directions
		cDirection,
		direction;

	Color::Filters filter = Color::Filters::Normal;

	/// A copy of the group pins to work on.
	vector<uint8_t> internalValues;

	/// A reference to the real group of elements.
	Group& group;

	virtual void calculateFrame() = 0;

};

}} /* namespace LEDSpicer::Animations */

#endif /* ANIMATION_HPP_ */
