/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Profile.hpp
 * @since     Jun 25, 2018
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2018 - 2020 Patricio A. Rossi (MeduZa)
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

#include "Element.hpp"
#include "Group.hpp"
#include "animations/FrameActor.hpp"
#include "utility/Color.hpp"
#include "inputs/Input.hpp"

#ifndef PROFILE_HPP_
#define PROFILE_HPP_ 1

#define REQUIRED_PARAM_PROFILE {"backgroundColor"}

namespace LEDSpicer {
namespace Devices {

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
		FrameActor* start,
		FrameActor* end
	):
		name(name),
		backgroundColor(backgroundColor),
		actual(start),
		start(start),
		end(end)
	{}

	virtual ~Profile() = default;

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

	const vector<Element::Item>& getAlwaysOnElements() const;
	const vector<Group::Item>& getAlwaysOnGroups() const;

	void addAlwaysOnElement(Element* element, const string& color);
	void addAlwaysOnGroup(Group* group, const string& color);

	/**
	 * Adds an input plugin to this profile.
	 * @param name
	 * @param input
	 */
	void addInput(string name, Input* input);

protected:

	/// Color to use when cleaning up.
	Color backgroundColor;

	/// Flag to know when the profile finished.
	bool running = true;

	/// Keeps the profile name.
	string name;

	FrameActor
		/// The current animation.
		* actual,
		/// The starting animation.
		* start,
		/// The ending animation.
		* end;

	/// List of animations by group to run.
	vector<Actor*> animations;

	/// Keeps a list of always on elements.
	vector<Element::Item> alwaysOnElements;

	/// Keeps a list of always on groups.
	vector<Group::Item> alwaysOnGroups;

	/// Input plugins
	umap<string, Input*> inputs;

private:

	/**
	 * Restarts the profile actors.
	 */
	void restartActors();
};

}} /* namespace LEDSpicer */

#endif /* PROFILE_HPP_ */
