/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      TransitionTest.cpp
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

#include <gtest/gtest.h>

#include "MockProfile.hpp"
#include "devices/transitions/Transition.hpp"
#include "devices/Profile.hpp"

namespace LEDSpicer::Devices::Transitions {

TEST(TransitionTest, Activate) {
	MockProfile currentProfile("current", Color::Off);
	MockProfile toProfile("to", Color::Off);
	currentProfile.addTestDummies();
	EXPECT_FALSE(currentProfile.gotReset());
	Transition transition;
	transition.activate(&currentProfile, &toProfile);
	// Current profile should have temporaries removed.
	EXPECT_TRUE(currentProfile.gotReset());
}

TEST(TransitionTest, RunWithToProfile) {
	MockProfile currentProfile("current", Color::Off);
	MockProfile toProfile("to", Color::Off);
	Transition transition;
	transition.activate(&currentProfile, &toProfile);
	EXPECT_FALSE(transition.run());
}

TEST(TransitionTest, RunWithoutToProfile) {
	MockProfile currentProfile("current", Color::Off);
	Transition transition;
	transition.activate(&currentProfile, nullptr);
	EXPECT_FALSE(transition.run());
}

TEST(TransitionTest, DrawConfig) {
	Transition transition;
	testing::internal::CaptureStdout();
	transition.drawConfig();
	string output = testing::internal::GetCapturedStdout();
	EXPECT_EQ(output, "Transition: None\n");
}

TEST(TransitionTest, Reusable) {
	MockProfile profile1("profile1", Color::Off);
	MockProfile profile2("profile2", Color::Off);
	MockProfile profile3("profile3", Color::Off);

	Transition transition;

	// First activation.
	transition.activate(&profile1, &profile2);
	EXPECT_FALSE(transition.run());

	// Reuse with different profiles.
	transition.activate(&profile2, &profile3);
	EXPECT_FALSE(transition.run());

	// Reuse for termination.
	transition.activate(&profile3, nullptr);
	EXPECT_FALSE(transition.run());
}

TEST(TransitionTest, StrToEffect) {
	EXPECT_EQ(Transition::str2effect("None"),      Transition::Effects::None);
	EXPECT_EQ(Transition::str2effect("FadeOutIn"), Transition::Effects::FadeOutIn);
	EXPECT_EQ(Transition::str2effect("Crossfade"), Transition::Effects::Crossfade);
	EXPECT_EQ(Transition::str2effect("Curtain"),   Transition::Effects::Curtain);
	EXPECT_EQ(Transition::str2effect("Invalid"),   Transition::Effects::None);
}

TEST(TransitionTest, EffectToStr) {
	EXPECT_EQ(Transition::effect2str(Transition::Effects::None),      "None");
	EXPECT_EQ(Transition::effect2str(Transition::Effects::FadeOutIn), "FadeOutIn");
	EXPECT_EQ(Transition::effect2str(Transition::Effects::Crossfade), "Crossfade");
	EXPECT_EQ(Transition::effect2str(Transition::Effects::Curtain),   "Curtain");
}

} // namespace
