/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Profile.hpp
 * @since     Jun 25, 2018
 * @author    Patricio A. Rossi (MeduZa)
 * @copyright Copyright © 2018 Patricio A. Rossi (MeduZa)
 */

#include "Element.hpp"
#include "Group.hpp"
#include "../animations/Actor.hpp"
#include "../utility/Color.hpp"

#ifndef PROFILE_HPP_
#define PROFILE_HPP_ 1

#define REQUIRED_PARAM_PROFILE {"backgroundColor"}

namespace LEDSpicer {
namespace Devices {

using Animations::Actor;

/**
 * LEDSpicer::Profile
 */
class Profile {

public:

	Profile(
		const Color& backgroundColor,
		Actor* start,
		Actor* end
	):
		backgroundColor(backgroundColor),
		actual(start),
		start(start),
		end(end)
	{}

	virtual ~Profile();

	void addAnimation(const string& animationName, const vector<Actor*>& animation);

	void drawConfig();

	/**
	 * Execute a frame.
	 */
	void runFrame();

	/**
	 * Leave it ready to restart.
	 */
	void reset();

	/**
	 * Initialize the ending sequence.
	 */
	void terminate();

	/**
	 * Returns true if the profile is transiting and the previous profile needs to draw.
	 * @return
	 */
	bool isTransiting() const;

	/**
	 * Returns true if the profile is running.
	 * @return
	 */
	bool isRunning() const;

	/**
	 * Returns a read only reference to the background color.
	 * @return
	 */
	const Color& getBackgroundColor() const;

	/**
	 * Return the number of animations.
	 * @return
	 */
	uint8_t getAnimationsCount() const;

protected:

	Color backgroundColor;

	bool running = true;

	Actor
		* actual,
		* start,
		* end;

	/// List of animations to run.
	umap<string, vector<Actor*>> animations;

	/// List of Elements by name that need the color to be changed.
	umap<string, const Color* const> elementsOverwrite;

	/// List of Groups by name that need the color to be changed.
	umap<string, const Color* const> groupsOverwrite;

};

}} /* namespace LEDSpicer */

#endif /* PROFILE_HPP_ */
