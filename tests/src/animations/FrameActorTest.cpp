/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      FrameActorTest.cpp
 * @since     Sep 2, 2025
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

	/**
	 * @param stepping the stepping to test.
	 */
	void setStepping(FrameActor::Stepping stepping) {
		this->stepping = stepping;
	}

	/**
	 * @return the stepping.
	 */
	FrameActor::Stepping  getStepping() {
		return stepping;
	}

	/**
	 * @return the current cycle.
	 */
	uint8_t getCycle() {
		return cycle;
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
		{"startAt", "10"},
		{"cycles",  "2"}
	};
	const vector<string> required = {"type", "group", "filter"};
	TestFrameActor actor(params, &group, required);
	EXPECT_EQ(actor.getStepping().frames, 0);
	EXPECT_EQ(actor.getStepping().frame, 0);
	EXPECT_EQ(actor.getCycle(), 0);
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

TEST(FrameActorTest, Cycles) {
	Group group("TestGroup", DEFAULT_COLOR);
	StringUMap params = {
		{"type",   "Test"},
		{"group",  "TestGroup"},
		{"filter", "Normal"},
		{"speed",  "VeryFast"},
		{"cycles", "2"}
	};
	const vector<string> required = {"type", "group", "filter"};
	TestFrameActor actor(params, &group, required);
	// 20 Frames.
	FrameActor::Stepping stepping;
	stepping.frames = FrameActor::calculateFramesBySpeed(Speed::Speeds::VeryFast);
	actor.setStepping(stepping);
	actor.restart();
	EXPECT_TRUE(actor.isRunning());
	EXPECT_EQ(actor.getCycle(), 0);

	// Simulate one cycle + 1 (20 frames from 0 to 0)
	for (int i = 0; i < 20; ++i) actor.draw();
	EXPECT_EQ(actor.getStepping().frame, 0);
	EXPECT_TRUE(actor.isRunning());
	EXPECT_EQ(actor.getCycle(), 1);

	// Simulate second cycle
	for (int i = 0; i < 20; ++i) actor.draw();
	EXPECT_EQ(actor.getStepping().frame, 0);
	EXPECT_EQ(actor.getCycle(), 2);
	EXPECT_FALSE(actor.isRunning());
	// this will be 3 instead of 2 due to the log protector.
	EXPECT_EQ(actor.getCycle(), 3);
}

// New test case for Cycles and Repeats
TEST(FrameActorTest, CyclesAndRepeats) {
	Group group("TestGroup", DEFAULT_COLOR);
	StringUMap params = {
		{"type",   "Test"},
		{"group",  "TestGroup"},
		{"filter", "Normal"},
		{"speed",  "VeryFast"},
		{"cycles", "2"},
		{"repeat", "2"}
	};
	const vector<string> required = {"type", "group", "filter"};
	TestFrameActor actor(params, &group, required);
	FrameActor::Stepping stepping;
	stepping.frames = FrameActor::calculateFramesBySpeed(Speed::Speeds::VeryFast);
	actor.setStepping(stepping);
	actor.restart();
	// Simulate one cycle + 1 (20 frames from 0 to 0)
	for (int i = 0; i < 20; ++i) actor.draw();
	EXPECT_EQ(actor.getStepping().frame, 0);
	EXPECT_TRUE(actor.isRunning());
	EXPECT_EQ(actor.getCycle(), 1);

	// Simulate second cycle
	for (int i = 0; i < 20; ++i) actor.draw();
	EXPECT_EQ(actor.getStepping().frame, 0);
	EXPECT_EQ(actor.getCycle(), 2);
	EXPECT_TRUE(actor.isRunning());

	// Simulate first cycle of second repeat
	for (int i = 0; i < 20; ++i) actor.draw();
	EXPECT_EQ(actor.getStepping().frame, 0);
	EXPECT_EQ(actor.getCycle(), 1);
	EXPECT_TRUE(actor.isRunning());

	// Simulate second cycle of second repeat
	for (int i = 0; i < 20; ++i) actor.draw();
	EXPECT_EQ(actor.getStepping().frame, 0);
	EXPECT_EQ(actor.getCycle(), 2);
	// At this point it ran 4 times 2x2.
	EXPECT_FALSE(actor.isRunning());
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
		{"cycles",    "2"},
		{"startTime", "1"},
		{"endTime",   "2"}
	};
	const vector<string> required = {"type", "group", "filter"};
	TestFrameActor actor(params, &group, required);
	FrameActor::Stepping stepping;
	stepping.frames = FrameActor::calculateFramesBySpeed(Speed::Speeds::Normal);
	actor.setStepping(stepping);
	actor.restart();
	// Run time = (cycles * frames / FPS) + startTime + (startAt * frames / (100 * FPS))
	// = (2 * 60 / 60) + 1 + (50 * 60 / (100 * 60)) = 2 + 1 + 0.5 = 3.5
	EXPECT_FLOAT_EQ(actor.getRunTime(), 3.5f);
}

TEST(FrameActorTest, DrawConfig) {
	Group group("TestGroup", DEFAULT_COLOR);
	StringUMap params = {
		{"type",    "Test"},
		{"group",   "TestGroup"},
		{"filter",  "Normal"},
		{"speed",   "Fast"},
		{"startAt", "10"},
		{"cycles",  "3"}
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
	EXPECT_NE(output.find("Speed: Fast"),            string::npos);
	EXPECT_NE(output.find("Will run for: 3 Cycles"), string::npos);
	EXPECT_NE(output.find("Start at Frame: 10"),     string::npos);
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
