/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Profile.hpp
 * @since     Jun 25, 2018
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2018 - 2026 Patricio A. Rossi (MeduZa)
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

#include "animations/Actor.hpp"
#include "utilities/Color.hpp"
#include "inputs/Input.hpp"

#pragma once

#define REQUIRED_PARAM_PROFILE {"backgroundColor"}

namespace LEDSpicer::Devices {

using Animations::Actor;
using Inputs::Input;

/**
 * LEDSpicer::Profile
 */
class Profile {

public:

	Profile(
		const string& name,
		const Color& backgroundColor
	) :
	name(name),
	backgroundColor(backgroundColor) {}

	virtual ~Profile();

	/**
	 * Adds an animation into the profile.
	 * @param animation
	 */
	void addAnimation(const vector<Actor*>& animation);

	/**
	 * Displays the internal information.
	 */
	void drawConfig() const;

	/**
	 * Execute a frame.
	 * @param advanceFrame
	 */
	void runFrame(bool advanceFrame = true);

	/**
	 * Leave the actor and inputs ready to go.
	 */
	void reset();

	/**
	 * Clean up any temporary handled item (element input or group)
	 */
	void removeTemporaries();

	/**
	 * Start inputs.
	 */
	void startInputs();

	/**
	 * Stop the inputs.
	 */
	void stopInputs();

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

	static void addTemporaryOnElement(const string& name, const Element::Item item);
	static void removeTemporaryOnElement(const string& name);
	static void removeTemporaryOnElements();

	static void addTemporaryOnGroup(const string& name, const Group::Item item);
	static void removeTemporaryOnGroup(const string& name);
	static void removeTemporaryOnGroups();

	/**
	 * Adds an input plugin to this profile.
	 * The Profile now owns the input.
	 * @param input
	 */
	void addInput(Input* input);

	/// Stores the default profile.
	static Profile* defaultProfile;

	/**
	 * Collect a distinct list of all LED pointers across all Actors.
	 * Duplicates are removed, order of first encounter is preserved.
	 *
	 * @return a list containing unique LED pointers.
	 */
	vector<uint8_t*> collectUniqueLeds() const;

protected:

	/// Keeps the profile name.
	const string name;

	/// Color to use when cleaning up.
	const Color& backgroundColor;

	/// List of animations to run.
	vector<Actor*> animations;

	/// Keeps a list of always on elements.
	vector<Element::Item> alwaysOnElements;

	/// Keeps a list of always on groups.
	vector<Group::Item> alwaysOnGroups;

	/// Input plugins
	vector<Input*> inputs;

	/// Keeps a list of temporary activated elements across profiles.
	static ElementItemUMap temporaryOnElements;

	/// Keeps a list of temporary activated on groups across profiles.
	static GroupItemUMap temporaryOnGroups;

};

using ProfilePtrUMap = unordered_map<string, Profile*>;
using ProfileUMap    = unordered_map<string, Profile>;

} // namespace
