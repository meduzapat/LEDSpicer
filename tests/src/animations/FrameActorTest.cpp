/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      FrameActorTest.cpp
 * @since     Sep 2, 2025
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
#include "animations/FrameActor.hpp"

using namespace LEDSpicer::Animations;
using namespace LEDSpicer::Devices;
using LEDSpicer::Utilities::Color;

// Global default color for tests.
const Color DEFAULT_COLOR(255, 255, 255);

namespace LEDSpicer::Animations {

/**
 * TestFrameActor
 * Minimal derived class for testing FrameActor.
 */
class TestFrameActor : public FrameActor {

public:

	TestFrameActor(
		StringUMap&           parameters,
		Group* const          group,
		const vector<string>& requiredParameters
	) : FrameActor(parameters, group, requiredParameters) {}

	void setStepping(FrameActor::Stepping stepping) {
		this->stepping = stepping;
	}

	FrameActor::Stepping  getStepping() {
		return stepping;
	}

protected:

	void calculateElements() override {
		affectAllElements(true);
	}
};

} // namespace

TEST(FrameActorTest, Constructor) {
	Actor::setFPS(60);
	Group group("TestGroup", DEFAULT_COLOR);
	StringUMap params = {
		{"type",    "Test"},
		{"group",   "TestGroup"},
		{"filter",  "Normal"},
		{"speed",   "Normal"},
		{"startAt", "10"}
	};
	const vector<string> required = {"type", "group", "filter"};
	TestFrameActor actor(params, &group, required);
	EXPECT_EQ(actor.getStepping().frames, 0);
	EXPECT_EQ(actor.getStepping().frame, 0);
}

TEST(FrameActorTest, SpeedVariations) {
	// Test VeryFast speed
	EXPECT_EQ(FrameActor::calculateFramesBySpeed(Speed::Speeds::VeryFast), 20); // FPS / 3 = 60 / 3 = 20

	// Test Fast speed with factor 2
	EXPECT_EQ(FrameActor::calculateFramesBySpeed(Speed::Speeds::Fast), 30); // FPS / 2 = 60 / 2 = 30

	// Test Normal speed
	EXPECT_EQ(FrameActor::calculateFramesBySpeed(Speed::Speeds::Normal), 60); // FPS = 60 = 60

	// Test Slow speed
	EXPECT_EQ(FrameActor::calculateFramesBySpeed(Speed::Speeds::Slow), 120); // FPS * 2 = 60 * 2 = 120

	// Test VerySlow speed
	EXPECT_EQ(FrameActor::calculateFramesBySpeed(Speed::Speeds::VerySlow), 180); // FPS * 3 = 60 * 3 = 180
}

TEST(FrameActorTest, StartAt) {
	Group group("TestGroup", DEFAULT_COLOR);
	StringUMap params = {
		{"type",    "Test"},
		{"group",   "TestGroup"},
		{"filter",  "Normal"},
		{"speed",   "Normal"},
		{"startAt", "50"}
	};
	const vector<string> required = {"type", "group", "filter"};
	TestFrameActor actor(params, &group, required);
	FrameActor::Stepping stepping;
	stepping.frames = FrameActor::calculateFramesBySpeed(Speed::Speeds::Normal);
	actor.setStepping(stepping);
	actor.restart();
	EXPECT_EQ(actor.getStepping().frame, 29); // (60 * (50-1)) / 100 = 29.4 -> 29
}

TEST(FrameActorTest, FrameAdvance) {
	Actor::setFPS(60);
	Group group("TestGroup", DEFAULT_COLOR);
	StringUMap params = {
		{"type",   "Test"},
		{"group",  "TestGroup"},
		{"filter", "Normal"},
		{"speed",  "Normal"}
	};
	const vector<string> required = {"type", "group", "filter"};
	TestFrameActor actor(params, &group, required);
	FrameActor::Stepping stepping;
	stepping.frames = FrameActor::calculateFramesBySpeed(Speed::Speeds::Normal);
	stepping.steps  = 2; // Set steps to 2 to test step progression
	actor.setStepping(stepping);
	actor.restart();

	// Initial state: frame 0, step 0
	EXPECT_TRUE(actor.isFirstFrame());
	EXPECT_TRUE(actor.isStartOfCycle());
	EXPECT_FALSE(actor.isLastFrame());
	EXPECT_FALSE(actor.isEndOfCycle());
	EXPECT_EQ(actor.getStepping().frame, 0);
	EXPECT_EQ(actor.getStepping().step, 0);

	// Advance one step (step 1, frame 0)
	actor.draw();
	EXPECT_TRUE(actor.isFirstFrame());
	EXPECT_FALSE(actor.isStartOfCycle());
	EXPECT_FALSE(actor.isLastFrame());
	EXPECT_FALSE(actor.isEndOfCycle());
	EXPECT_EQ(actor.getStepping().frame, 0);
	EXPECT_EQ(actor.getStepping().step, 1);

	// Advance to next frame (step 0, frame 1)
	actor.draw();
	EXPECT_FALSE(actor.isFirstFrame());
	EXPECT_FALSE(actor.isStartOfCycle());
	EXPECT_FALSE(actor.isLastFrame());
	EXPECT_FALSE(actor.isEndOfCycle());
	EXPECT_EQ(actor.getStepping().frame, 1);
	EXPECT_EQ(actor.getStepping().step, 0);

	// Advance to last frame, first step (frame 59, step 0)
	for (int i = 0; i < 58 * 2; ++i) actor.draw(); // 58 frames * 2 steps
	EXPECT_FALSE(actor.isFirstFrame());
	EXPECT_FALSE(actor.isStartOfCycle());
	EXPECT_TRUE(actor.isLastFrame());
	EXPECT_FALSE(actor.isEndOfCycle());
	EXPECT_EQ(actor.getStepping().frame, 59);
	EXPECT_EQ(actor.getStepping().step, 0);

	// Advance to last frame, last step (frame 59, step 1)
	actor.draw();
	EXPECT_FALSE(actor.isFirstFrame());
	EXPECT_FALSE(actor.isStartOfCycle());
	EXPECT_TRUE(actor.isLastFrame());
	EXPECT_TRUE(actor.isEndOfCycle());
	EXPECT_EQ(actor.getStepping().frame, 59);
	EXPECT_EQ(actor.getStepping().step, 1);

	// Advance to first frame of next cycle (frame 0, step 0)
	actor.draw();
	EXPECT_TRUE(actor.isFirstFrame());
	EXPECT_TRUE(actor.isStartOfCycle());
	EXPECT_FALSE(actor.isLastFrame());
	EXPECT_FALSE(actor.isEndOfCycle());
	EXPECT_EQ(actor.getStepping().frame, 0);
	EXPECT_EQ(actor.getStepping().step, 0);
}

TEST(FrameActorTest, RunTime) {
	Group group("TestGroup", DEFAULT_COLOR);
	StringUMap params = {
		{"type",      "Test"},
		{"group",     "TestGroup"},
		{"filter",    "Normal"},
		{"speed",     "Normal"},
		{"startAt",   "50"},
		{"startTime", "1"},
		{"endTime",   "2"}
	};
	const vector<string> required = {"type", "group", "filter"};
	TestFrameActor actor(params, &group, required);
	FrameActor::Stepping stepping;
	stepping.frames = FrameActor::calculateFramesBySpeed(Speed::Speeds::Normal);
	actor.setStepping(stepping);
	actor.restart();
	// Run time = startTime + endTime + (startAt * frames) / (100 * FPS)
	// = 1 + 2 + (50 * 60) / (100 * 60) = 3 + 0.5 = 3.5
	EXPECT_FLOAT_EQ(actor.getRunTime(), 3.5f);
}

TEST(FrameActorTest, DrawConfig) {
	Group group("TestGroup", DEFAULT_COLOR);
	StringUMap params = {
		{"type",    "Test"},
		{"group",   "TestGroup"},
		{"filter",  "Normal"},
		{"speed",   "Fast"},
		{"startAt", "10"}
	};
	const vector<string> required = {"type", "group", "filter"};
	TestFrameActor actor(params, &group, required);
	actor.restart();
	Log::logToStdTerm();
	Log::setLogLevel(LOG_DEBUG);
	testing::internal::CaptureStdout();
	Log::setLogLevel(LOG_ERR);
	actor.drawConfig();
	string output = testing::internal::GetCapturedStdout();
	EXPECT_NE(output.find("Speed: Fast"),        string::npos);
	EXPECT_NE(output.find("Start at Frame: 10"), string::npos);
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
