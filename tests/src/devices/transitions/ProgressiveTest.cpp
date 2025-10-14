/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      ProgressiveTest.cpp
 * @since     Sep 22, 2025
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
#include <cmath>

#include "MockProfile.hpp"
#include "devices/transitions/Progressive.hpp"
#include "devices/Profile.hpp"

// Mock subclass to test abstract base (implements calculate as no-op).
class MockProgressive : public LEDSpicer::Devices::Transitions::Progressive {

public:

	using Progressive::Progressive;

	float getIncrease() const {
		return increase;
	}

protected:

	void calculate() override {}
};

namespace LEDSpicer::Devices::Transitions {

TEST(ProgressiveTest, ConstructorWithInvalidSpeed) {
	MockProfile current("current", Color::Off);
	MockProfile to("to", Color::Off);
	EXPECT_THROW(MockProgressive(&current, "Invalid"), Error);
}

TEST(ProgressiveTest, RunVeryFast) {
	MockProfile currentProfile("current", Color::Off);
	MockProfile toProfile("to", Color::Off);
	MockProgressive transition(&currentProfile, "VeryFast");
	transition.setTarget(&toProfile);
	int steps = 0;
	while (transition.run()) steps++;
	EXPECT_EQ(steps, static_cast<int>(std::ceil(100.0f / transition.getIncrease())));
}

TEST(ProgressiveTest, RunNormal) {
	MockProfile currentProfile("current", Color::Off);
	MockProfile toProfile("to", Color::Off);
	MockProgressive transition(&currentProfile, "Normal");
	transition.setTarget(&toProfile);
	int steps = 0;
	while (transition.run()) steps++;
	EXPECT_EQ(steps, static_cast<int>(std::ceil(100.0f / transition.getIncrease())));
}

TEST(ProgressiveTest, RunVerySlow) {
	MockProfile current("current", Color::Off);
	MockProfile toProfile("to", Color::Off);
	MockProgressive transition(&current, "VerySlow");
	transition.setTarget(&toProfile);
	int steps = 0;
	while (transition.run()) steps++;
	EXPECT_EQ(steps, static_cast<int>(std::ceil(100.0f / transition.getIncrease())));
}

TEST(ProgressiveTest, RunWithoutToProfile) {
	MockProfile currentProfile("current", Color::Off);
	MockProgressive transition(&currentProfile, "Normal");
	int steps = 0;
	while (transition.run()) steps++;
	EXPECT_EQ(steps, static_cast<int>(std::ceil(100.0f / transition.getIncrease())));
}

TEST(ProgressiveTest, DrawConfig) {
	MockProfile currentProfile("current", Color::Off);
	MockProfile toProfile("to", Color::Off);
	MockProgressive transition(&currentProfile, "Slow");
	transition.setTarget(&toProfile);
	testing::internal::CaptureStdout();
	transition.drawConfig();
	string output = testing::internal::GetCapturedStdout();
	EXPECT_EQ(output, "Speed: Slow\n");
}

} // namespace
