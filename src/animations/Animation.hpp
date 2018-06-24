/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file		Animation.hpp
 * @ingroup
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
#include "../utility/Error.hpp"
#include "../utility/Log.hpp"

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
class Animation {

public:

	enum Directions : uint8_t {None, Forward, Backward, Bouncing = 4};

	enum Effects    : uint8_t {None, SlowFade, Fade, FastFade};

	Animation(const umap<string, string>& parameters, Group& layout);

	virtual ~Animation();

	void drawFrame();

protected:

	uint8_t speed = 0;

	Color::Filter filter = Color::Filter::Color;

	/// A copy of the group elements to work.
	Group frame;

	/// A reference to the real group of elements.
	Group& group;

	virtual void calculateFrame() = 0;

};

}} /* namespace LEDSpicer::Animations */

#endif /* ANIMATION_HPP_ */
