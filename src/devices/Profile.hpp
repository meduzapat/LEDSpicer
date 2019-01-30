/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Profile.hpp
 * @since     Jun 25, 2018
 * @author    Patricio A. Rossi (MeduZa)
 * @copyright Copyright © 2018 Patricio A. Rossi (MeduZa)
 */

#include "Element.hpp"
#include "Group.hpp"
#include "animations/Actor.hpp"
#include "utility/Color.hpp"

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

	/**
	 * Structure to handle always on elements.
	 */
	struct ElementItem {
		Element* element = nullptr;
		const Color* color = nullptr;
		Color::Filters filter;
	};

	/**
	 * Structure to handle always on groups.
	 */
	struct GroupItem {
		const Group* group = nullptr;
		const Color* color = nullptr;
		Color::Filters filter;
	};

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

	virtual ~Profile() = default;

	void addAnimation(const vector<Actor*>& animation);

	void drawConfig();

	/**
	 * Execute a frame.
	 */
	void runFrame();

	/**
	 * Leave the actor ready to run (no start sequence).
	 */
	void reset();

	/**
	 * Leave the actor ready to run from the start sequence.
	 */
	void restart();

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

	const vector<ElementItem>& getAlwaysOnElements() const;

	void addAlwaysOnElement(Element* element, const string& color);

	const vector<GroupItem>& getAlwaysOnGroups() const;

	void addAlwaysOnGroup(Group* group, const string& color);

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

	/// List of animations by group to run.
	vector<Actor*> animations;

	/// Keeps a list of always on elements.
	vector<ElementItem> alwaysOnElements;

	/// Keeps a list of always on groups.
	vector<GroupItem> alwaysOnGroups;

private:

	/**
	 * Restarts the profile actors.
	 */
	void restartActors();
};

}} /* namespace LEDSpicer */

#endif /* PROFILE_HPP_ */
