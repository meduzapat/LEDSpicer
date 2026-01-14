/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Transition.hpp
 * @since     Sep 20, 2025
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

#include "devices/Profile.hpp"

#pragma once

namespace LEDSpicer::Devices::Transitions {

using LEDSpicer::Devices::Profile;

/**
 * LEDSpicer::Transition
 * Base class to handle transitions between profiles.
 * Subclasses can override run() to implement specific effects.
 */
class Transition {

public:

	/**
	 * Enum for different transition effects.
	 */
	enum class Effects : uint8_t {
		None,      /// Instantaneous transition, no effect.
		FadeOutIn, /// Fading out the current profile and fading in the new one.
		Crossfade, /// Crossfading from one profile to the other.
		Curtain    /// A curtain closing and opening to the new profile.
	};

	Transition() = default;

	virtual ~Transition() = default;

	/**
	 * Activates the transition between two profiles.
	 * @param from The profile to transition from.
	 * @param to The profile to transition to (nullptr for termination).
	 */
	virtual void activate(Profile* from, Profile* to);

	/**
	 * Performs one step of the transition.
	 * Subclasses override this to implement multi-step effects.
	 *
	 * @return true if the transition is still active (more steps needed), false if complete.
	 */
	virtual bool run();

	/**
	 * Clean up after transition ends.
	 */
	virtual void deactivate();

	/**
	 * @return The name of the Transition.
	 */
	virtual void drawConfig() const;

	/**
	 * Converts a string into a transition effect.
	 * @param effect
	 * @return the Effects value
	 */
	static Effects str2effect(const string& effect);

	/**
	 * Converts a transition effect into a string.
	 * @param effect
	 * @return the string representation of Effects.
	 */
	static string effect2str(Effects effect);

protected:

	/**
	 * Resets the transition state for reuse.
	 */
	virtual void reset();

	Profile
		/// The profile transitioning from.
		* current = nullptr,
		/// The profile transitioning to, or nullptr for termination.
		* to = nullptr;
};

} // namespace
