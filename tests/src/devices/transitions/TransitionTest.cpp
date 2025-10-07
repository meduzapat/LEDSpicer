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

#include "devices/transitions/Transition.hpp"
#include "devices/Profile.hpp"

// Mock Profile class for testing.
struct MockProfile : public LEDSpicer::Devices::Profile {

	using Profile::Profile;

	virtual ~MockProfile() = default;

	void addTestDummies() {
		temporaryOnElements.emplace("test", Element::Item{&element, &Color::On, Color::Filters::Normal, 0});
		temporaryOnGroups.emplace("test", Group::Item{&group, &Color::On, Color::Filters::Normal, 0});
	}

	bool gotReset() const {
		return temporaryOnElements.empty() and temporaryOnGroups.empty();
	}

	uint8_t pin = 0;
	Element element{"name", &pin, Color::On, 0, 100};
	Group   group{Color::On};

};

namespace LEDSpicer::Devices::Transitions {

TEST(TransitionTest, Constructors) {
	MockProfile currentProfile("current", Color::Off);
	MockProfile toProfile("to", Color::Off);
	currentProfile.addTestDummies();
	EXPECT_FALSE(currentProfile.gotReset());
	Transition transition(&currentProfile);
	transition.setTarget(&toProfile);
	EXPECT_TRUE(currentProfile.gotReset());
}

TEST(TransitionTest, RunWithToProfile) {
	MockProfile currentProfile("current", Color::Off);
	MockProfile toProfile("to", Color::Off);
	Transition transition(&currentProfile);
	transition.setTarget(&toProfile);
	EXPECT_FALSE(transition.run());
}

TEST(TransitionTest, RunWithoutToProfile) {
	MockProfile currentProfile("current", Color::Off);
	Transition transition(&currentProfile);
	EXPECT_FALSE(transition.run());
}

TEST(TransitionTest, DrawConfig) {
	MockProfile currentProfile("current", Color::Off);
	MockProfile toProfile("to", Color::Off);
	Transition transition(&currentProfile);
	transition.setTarget(&toProfile);
	testing::internal::CaptureStdout();
	transition.drawConfig();
	string output = testing::internal::GetCapturedStdout();
	EXPECT_EQ(output, "Transition: None\n");
}

} // namespace
