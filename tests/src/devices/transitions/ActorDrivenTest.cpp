/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      ActorDrivenTest.cpp
 * @since     Oct 11, 2025
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
#include "MockDirectionActor.hpp"
#include "devices/transitions/ActorDriven.hpp"

using namespace LEDSpicer::Devices;
using namespace LEDSpicer::Devices::Transitions;
using namespace LEDSpicer::Animations;
using namespace LEDSpicer::Utilities;

namespace {

const Color DEFAULT_COLOR(255, 255, 255);

MockDirectionActor* createTestActor(Group* group) {
	StringUMap params = {
		{"type",      "Test"},
		{"group",     "TestGroup"},
		{"filter",    "Normal"},
		{"speed",     "Normal"},
		{"direction", "Forward"}
	};
	const vector<string> required = {"type", "group", "filter", "speed", "direction"};
	Actor::setFPS(60);
	auto* actor = new MockDirectionActor(params, group, required);
	FrameActor::Stepping stepping;
	stepping.frames = FrameActor::calculateFramesBySpeed(Speed::Speeds::Normal);
	actor->setStepping(stepping);
	return actor;
}

} // namespace

TEST(ActorDrivenTest, Constructor) {
	MockProfile  currentProfile("current", Color::Off);
	Group        group("TestGroup", DEFAULT_COLOR);
	auto*        actor(createTestActor(&group));
	ActorDriven  transition(&currentProfile, actor);
	EXPECT_FALSE(actor->wasRestarted());
}

TEST(ActorDrivenTest, Reset) {
	MockProfile currentProfile("current", Color::Off);
	MockProfile to("to", Color::Off);
	Group       group("TestGroup", DEFAULT_COLOR);
	auto*       actor(createTestActor(&group));
	ActorDriven transition(&currentProfile, actor);

	currentProfile.addTestDummies();
	EXPECT_FALSE(currentProfile.gotReset());
	EXPECT_FALSE(actor->wasRestarted());

	transition.setTarget(&to);

	EXPECT_TRUE(currentProfile.gotReset());
	EXPECT_TRUE(actor->wasRestarted());
}

TEST(ActorDrivenTest, DrawConfig) {
	MockProfile currentProfile("current", Color::Off);
	Group       group("TestGroup", DEFAULT_COLOR);
	auto*       actor      = createTestActor(&group);
	ActorDriven transition(&currentProfile, actor);

	testing::internal::CaptureStdout();
	transition.drawConfig();
	string output = testing::internal::GetCapturedStdout();

	EXPECT_NE(output.find("Actor:"), string::npos);
	EXPECT_NE(output.find("Direction: Forward"), string::npos);
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
