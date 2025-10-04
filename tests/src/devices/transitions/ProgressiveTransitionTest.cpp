/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      ProgressiveTransitionTest.cpp
 * @since     Sep 22, 2025
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2018 - 2025 Patricio A. Rossi (MeduZa)
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
#include <cmath>

#include "devices/transitions/ProgressiveTransition.hpp"
#include "devices/Profile.hpp"

// Reuse MockProfile from TransitionTest.cpp.
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

// Mock subclass to test abstract base (implements calculate as no-op).
class MockProgressiveTransition : public LEDSpicer::Devices::Transitions::ProgressiveTransition {

public:

	using ProgressiveTransition::ProgressiveTransition;

	float getIncrease() const {
		return increase;
	}

protected:

	void calculate() override {}
};

namespace LEDSpicer::Devices::Transitions {

TEST(ProgressiveTransitionTest, ConstructorWithInvalidSpeed) {
	MockProfile current("current", Color::Off);
	MockProfile to("to", Color::Off);
	EXPECT_THROW(MockProgressiveTransition(&current, "Invalid"), Error);
}

TEST(ProgressiveTransitionTest, RunVeryFast) {
	MockProfile currentProfile("current", Color::Off);
	MockProfile toProfile("to", Color::Off);
	MockProgressiveTransition transition(&currentProfile, "VeryFast");
	transition.setTarget(&toProfile);
	int steps = 0;
	while (transition.run()) steps++;
	EXPECT_EQ(steps, static_cast<int>(std::ceil(100.0f / transition.getIncrease())));
}

TEST(ProgressiveTransitionTest, RunNormal) {
	MockProfile currentProfile("current", Color::Off);
	MockProfile toProfile("to", Color::Off);
	MockProgressiveTransition transition(&currentProfile, "Normal");
	transition.setTarget(&toProfile);
	int steps = 0;
	while (transition.run()) steps++;
	EXPECT_EQ(steps, static_cast<int>(std::ceil(100.0f / transition.getIncrease())));
}

TEST(ProgressiveTransitionTest, RunVerySlow) {
	MockProfile current("current", Color::Off);
	MockProfile toProfile("to", Color::Off);
	MockProgressiveTransition transition(&current, "VerySlow");
	transition.setTarget(&toProfile);
	int steps = 0;
	while (transition.run()) steps++;
	EXPECT_EQ(steps, static_cast<int>(std::ceil(100.0f / transition.getIncrease())));
}

TEST(ProgressiveTransitionTest, RunWithoutToProfile) {
	MockProfile currentProfile("current", Color::Off);
	MockProgressiveTransition transition(&currentProfile, "Normal");
	int steps = 0;
	while (transition.run()) steps++;
	EXPECT_EQ(steps, static_cast<int>(std::ceil(100.0f / transition.getIncrease())));
}

TEST(ProgressiveTransitionTest, DrawConfig) {
	MockProfile currentProfile("current", Color::Off);
	MockProfile toProfile("to", Color::Off);
	MockProgressiveTransition transition(&currentProfile, "Slow");
	transition.setTarget(&toProfile);
	testing::internal::CaptureStdout();
	transition.drawConfig();
	string output = testing::internal::GetCapturedStdout();
	EXPECT_EQ(output, "Speed: Slow\n");
}

} // namespace
