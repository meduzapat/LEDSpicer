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
		const string& name,
		const Color& backgroundColor,
		Actor* start,
		Actor* end
	):
		name(name),
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
	 * @return true if the profile is transiting and the previous profile needs to draw.
	 */
	bool isTransiting() const;

	/**
	 * @return true if the profile is running the termination animation.
	 */
	bool isTerminating() const;

	/**
	 * @return true if the profile is running the stating animation.
	 */
	bool isStarting() const;

	/**
	 * @return true if the profile is running.
	 */
	bool isRunning() const;

	/**
	 * @return a read only reference to the background color.
	 */
	const Color& getBackgroundColor() const;

	/**
	 * @return the number of animations.
	 */
	uint8_t getAnimationsCount() const;

	/**
	 * @return the Profile name.
	 */
	const string& getName() const;

	const umap<string, const Color*>& getElementsOverwrite() const;

	void addElementOverwrite(const string& element ,const string& color);

	const umap<string, const Color*>& getGroupsOverwrite() const;

	void addGroupOverwrite(const string& group, const string& color);

protected:

	/// Color to use when cleaning up.
	Color backgroundColor;

	/// Flag to know when the profile finished.
	bool running = true;

	/// Keeps the profile name.
	string name;

	Actor
		/// The current animation.
		* actual,
		/// The starting animation.
		* start,
		/// The ending animation.
		* end;

	/// List of animations to run.
	umap<string, vector<Actor*>> animations;

	/// List of Elements by name that need the color to be changed.
	umap<string, const Color*> elementsOverwrite;

	/// List of Groups by name that need the color to be changed.
	umap<string, const Color*> groupsOverwrite;

private:

	/**
	 * Restarts the profile actors.
	 */
	void restartActors();
};

}} /* namespace LEDSpicer */

#endif /* PROFILE_HPP_ */
