/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Profile.hpp
 * @since     Jun 25, 2018
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2018 - 2024 Patricio A. Rossi (MeduZa)
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

#include <chrono>
using std::chrono::milliseconds;
using std::chrono::time_point;
#include "animations/FrameActor.hpp"
#include "utility/Color.hpp"
#include "inputs/Input.hpp"

#ifndef PROFILE_HPP_
#define PROFILE_HPP_ 1

#define REQUIRED_PARAM_PROFILE {"backgroundColor"}

namespace LEDSpicer::Devices {

using Animations::FrameActor;
using Animations::Actor;
using Inputs::Input;

/**
 * LEDSpicer::Profile
 */
class Profile {

public:

	Profile(
		const string& name,
		const Color& backgroundColor,
		const vector<Actor*>& startTransitions,
		const vector<Actor*>& endTransitions,
		const milliseconds startTransitionElementsOnAt,
		const milliseconds endTransitionElementsOffAt
	);

	virtual ~Profile();

	void addAnimation(const vector<Actor*>& animation);

	/**
	 * Displays the internal information.
	 */
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
	 * Stop the the current profile (no end sequence).
	 */
	void stop();

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
	 * Whatever the always
	 * @return
	 */
	bool displayElements() const;

	/**
	 * @return a read only reference to the background color.
	 */
	const Color& getBackgroundColor() const;

	/**
	 * @return the Profile name.
	 */
	const string& getName() const;

	void addAlwaysOnElement(Element* element, const Color& color, const Color::Filters& filter);
	void addAlwaysOnGroup(Group* group, const Color& color, const Color::Filters& filter);

	static void addTemporaryAlwaysOnElement(const string name, const Element::Item item);
	static void removeTemporaryAlwaysOnElement(const string name);
	static void removeTemporaryAlwaysOnElements();

	static void addTemporaryAlwaysOnGroup(const string name, const Group::Item item);
	static void removeTemporaryAlwaysOnGroup(const string name);
	static void removeTemporaryAlwaysOnGroups();

	/**
	 * Adds an input plugin to this profile.
	 * @param input
	 */
	void addInput(Input* input);

	/**
	 * Sets the global flags pointer.
	 * @param flags
	 */
	static void setGlobalFlags(uint8_t* flags);

protected:

	/// Keeps the profile name.
	string name;

	/// Color to use when cleaning up.
	Color backgroundColor;

	/// What the profile is doing.
	vector<Actor*>* currentActors;

	/// List of animations to run.
	vector<Actor*> animations;

	/// List of animations to run at start.
	vector<Actor*> startTransitions;

	/// Start transition time when always on elements are displayed.
	milliseconds startTransitionElementsOnAt;

	/// List of animations to run at end.
	vector<Actor*> endTransitions;

	/// End transition time when always on elements get hidden.
	milliseconds endTransitionElementsOffAt;

	/// Keeps a list of always on elements.
	vector<Element::Item> alwaysOnElements;

	/// Keeps a list of always on groups.
	vector<Group::Item> alwaysOnGroups;

	/// Input plugins
	vector<Input*> inputs;

	/// Keeps the ending time milliseconds.
	time_point<std::chrono::system_clock>* transitionEndTime = nullptr;

	/// Keeps a list of temporary activated always on elements.
	static umap<string, Element::Item> temporaryAlwaysOnElements;

	/// Keeps a list of temporary activated always on groups.
	static umap<string, Group::Item> temporaryAlwaysOnGroups;

private:

	/// To be used when there is a start or end transition and the elements needs to show up.
	float
		elementProgress   = 0,
		startStepProgress = 0,
		endStepProgress   = 0;

	/**
	 * Restarts the profile actors.
	 */
	void restartActors();

	void runAlwaysOnElements(bool force);
};

} /* namespace */

#endif /* PROFILE_HPP_ */
