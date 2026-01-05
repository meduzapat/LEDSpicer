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

// Mock subclass to test abstract base.
class MockProgressive : public LEDSpicer::Devices::Transitions::Progressive {

public:

	using Progressive::Progressive;

	uint8_t getStep() const {
		return step;
	}

protected:

	void calculate() override {}
};

namespace LEDSpicer::Devices::Transitions {

TEST(ProgressiveTest, Run) {
	MockProfile currentProfile("current", Color::Off);
	MockProfile toProfile("to", Color::Off);
	MockProgressive transition("Normal");
	transition.activate(&currentProfile, &toProfile);
	int steps = 0;
	while (transition.run()) steps++;
	EXPECT_EQ(steps, static_cast<int>(std::ceil(100.0f / transition.getStep())));
}

TEST(ProgressiveTest, Reusable) {
	MockProfile profile1("profile1", Color::Off);
	MockProfile profile2("profile2", Color::Off);
	MockProgressive transition("VeryFast");

	transition.activate(&profile1, &profile2);
	while (transition.run()) {}

	// Reuse — should reset and run again.
	transition.activate(&profile2, &profile1);
	int steps = 0;
	while (transition.run()) steps++;
	EXPECT_EQ(steps, static_cast<int>(std::ceil(100.0f / transition.getStep())));
}

TEST(ProgressiveTest, DrawConfig) {
	MockProgressive transition("Slow");
	testing::internal::CaptureStdout();
	transition.drawConfig();
	string output = testing::internal::GetCapturedStdout();
	EXPECT_EQ(output, "Speed: Slow\n");
}

} // namespace
