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

/// Global default color for tests.
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

	/// Sets stepping for testing.
	void setStepping(FrameActor::Stepping s) {
		stepping = s;
	}

	/// Returns current stepping state.
	const FrameActor::Stepping& getStepping() const {
		return stepping;
	}

protected:

	void calculateElements() override {
		affectAllElements(true);
	}
};

} // namespace

/// Shared normal stepping setup.
static FrameActor::Stepping makeNormalStepping(uint16_t steps = 0) {
	FrameActor::Stepping s;
	s.frames = FrameActor::calculateFramesBySpeed(Speed::Speeds::Normal);
	s.steps  = steps;
	return s;
}

/// Shared actor factory.
static TestFrameActor makeActor(
	Group&             group,
	const string&      speed   = "Normal",
	uint8_t            startAt = 0,
	uint16_t           cycles  = 0,
	float              endTime = 0
) {
	StringUMap params {
		{"type",   "Test"},
		{"group",  "TestGroup"},
		{"filter", "Normal"},
		{"speed",  speed},
	};
	if (startAt) params["startAt"] = to_string(startAt);
	if (cycles)  params["cycles"]  = to_string(cycles);
	if (endTime) params["endTime"] = to_string(endTime);
	const vector<string> required {"type", "group", "filter"};
	return TestFrameActor(params, &group, required);
}

TEST(FrameActorTest, Constructor) {
	Actor::setFPS(60);
	Group group("TestGroup", DEFAULT_COLOR);
	auto  actor = makeActor(group, "Normal", 10);
	// stepping not set yet — frames stays 0 until setStepping.
	EXPECT_EQ(actor.getStepping().frames, 0);
	EXPECT_EQ(actor.getStepping().frame,  0);
}

TEST(FrameActorTest, SpeedVariations) {
	Actor::setFPS(60);
	EXPECT_EQ(FrameActor::calculateFramesBySpeed(Speed::Speeds::VeryFast), 20);  // FPS / 3
	EXPECT_EQ(FrameActor::calculateFramesBySpeed(Speed::Speeds::Fast),     30);  // FPS / 2
	EXPECT_EQ(FrameActor::calculateFramesBySpeed(Speed::Speeds::Normal),   60);  // FPS
	EXPECT_EQ(FrameActor::calculateFramesBySpeed(Speed::Speeds::Slow),     120); // FPS * 2
	EXPECT_EQ(FrameActor::calculateFramesBySpeed(Speed::Speeds::VerySlow), 180); // FPS * 3
}

TEST(FrameActorTest, StartAt) {
	Actor::setFPS(60);
	Group group("TestGroup", DEFAULT_COLOR);
	auto  actor = makeActor(group, "Normal", 50);
	actor.setStepping(makeNormalStepping());
	actor.restart();
	// (60 * (50 - 1)) / 100 = 29.4 → 29
	EXPECT_EQ(actor.getStepping().frame, 29);
}

TEST(FrameActorTest, FrameAdvance) {
	Actor::setFPS(60);
	Group group("TestGroup", DEFAULT_COLOR);
	auto  actor   = makeActor(group);
	auto  stepping = makeNormalStepping(2);
	actor.setStepping(stepping);
	actor.restart();

	const uint16_t lastFrame = stepping.frames - 1;

	// Initial state: frame 0, step 0.
	EXPECT_TRUE(actor.isFirstFrame());
	EXPECT_TRUE(actor.isStartOfCycle());
	EXPECT_FALSE(actor.isLastFrame());
	EXPECT_FALSE(actor.isEndOfCycle());
	EXPECT_EQ(actor.getStepping().frame, 0);
	EXPECT_EQ(actor.getStepping().step,  0);

	// Advance one step (step 1, frame 0).
	actor.draw();
	EXPECT_TRUE(actor.isFirstFrame());
	EXPECT_FALSE(actor.isStartOfCycle());
	EXPECT_FALSE(actor.isLastFrame());
	EXPECT_FALSE(actor.isEndOfCycle());
	EXPECT_EQ(actor.getStepping().frame, 0);
	EXPECT_EQ(actor.getStepping().step,  1);

	// Advance to next frame (step 0, frame 1).
	actor.draw();
	EXPECT_FALSE(actor.isFirstFrame());
	EXPECT_FALSE(actor.isStartOfCycle());
	EXPECT_FALSE(actor.isLastFrame());
	EXPECT_FALSE(actor.isEndOfCycle());
	EXPECT_EQ(actor.getStepping().frame, 1);
	EXPECT_EQ(actor.getStepping().step,  0);

	// Advance to last frame, first step.
	for (uint16_t i = 0; i < (lastFrame - 1) * stepping.steps; ++i) actor.draw();
	EXPECT_FALSE(actor.isFirstFrame());
	EXPECT_FALSE(actor.isStartOfCycle());
	EXPECT_TRUE(actor.isLastFrame());
	EXPECT_FALSE(actor.isEndOfCycle());
	EXPECT_EQ(actor.getStepping().frame, lastFrame);
	EXPECT_EQ(actor.getStepping().step,  0);

	// Advance to last frame, last step.
	actor.draw();
	EXPECT_TRUE(actor.isLastFrame());
	EXPECT_TRUE(actor.isEndOfCycle());
	EXPECT_EQ(actor.getStepping().frame, lastFrame);
	EXPECT_EQ(actor.getStepping().step,  1);

	// Advance to first frame of next cycle.
	actor.draw();
	EXPECT_TRUE(actor.isFirstFrame());
	EXPECT_TRUE(actor.isStartOfCycle());
	EXPECT_FALSE(actor.isLastFrame());
	EXPECT_FALSE(actor.isEndOfCycle());
	EXPECT_EQ(actor.getStepping().frame, 0);
	EXPECT_EQ(actor.getStepping().step,  0);
}

TEST(FrameActorTest, CyclesStopsAfterOne) {
	Actor::setFPS(60);
	Group group("TestGroup", DEFAULT_COLOR);
	auto  actor   = makeActor(group, "Normal", 0, 1);
	auto  stepping = makeNormalStepping();
	actor.setStepping(stepping);
	actor.restart();

	// Run until one draw before the last frame.
	EXPECT_TRUE(actor.isRunning());
	for (uint16_t i = 0; i < stepping.frames - 2; ++i) actor.draw();
	EXPECT_TRUE(actor.isRunning());

	// This draw advances to the last frame, completing the cycle.
	actor.draw();
	// cycle >= cycles → stopped.
	EXPECT_FALSE(actor.isRunning());
	EXPECT_FALSE(actor.isRunning());
}

TEST(FrameActorTest, CyclesStopsAfterTwo) {
	Actor::setFPS(60);
	Group group("TestGroup", DEFAULT_COLOR);
	auto  actor    = makeActor(group, "Normal", 0, 2);
	auto  stepping = makeNormalStepping();
	actor.setStepping(stepping);
	actor.restart();

	// Cycle 1: frames-1 draws advance to last frame (cycle=1), then 1 wrap draw.
	for (uint16_t i = 0; i < stepping.frames; ++i) actor.draw();
	EXPECT_TRUE(actor.isRunning()); // cycle=1, frame=0

	// Cycle 2: run until one draw before the last frame.
	for (uint16_t i = 0; i < stepping.frames - 2; ++i) actor.draw();
	EXPECT_TRUE(actor.isRunning());

	// This draw advances to the last frame, completing cycle 2.
	actor.draw();

	// Stopped after 2 cycles.
	EXPECT_FALSE(actor.isRunning());
	EXPECT_FALSE(actor.isRunning());
}

TEST(FrameActorTest, CyclesResetOnRestart) {
	Actor::setFPS(60);
	Group group("TestGroup", DEFAULT_COLOR);
	auto  actor   = makeActor(group, "Normal", 0, 1);
	auto  stepping = makeNormalStepping();
	actor.setStepping(stepping);
	actor.restart();

	// Exhaust cycles.
	for (uint16_t i = 0; i < stepping.frames; ++i) actor.draw();
	EXPECT_FALSE(actor.isRunning());

	// restart() resets cycle counter.
	actor.restart();
	EXPECT_TRUE(actor.isRunning());
}

TEST(FrameActorTest, RunTimeNoCycles) {
	Actor::setFPS(60);
	Group group("TestGroup", DEFAULT_COLOR);

	// No cycles, no endTime → ∞.
	{
		auto actor   = makeActor(group);
		auto stepping = makeNormalStepping();
		actor.setStepping(stepping);
		EXPECT_FLOAT_EQ(actor.getRunTime(), 0.0f);
	}

	// cycles=2, no endTime → cycleTime * 2.
	{
		auto actor   = makeActor(group, "Normal", 0, 2);
		auto stepping = makeNormalStepping();
		actor.setStepping(stepping);
		// cycleTime = 60 / 60 = 1.0s → 2 cycles = 2.0s
		EXPECT_FLOAT_EQ(actor.getRunTime(), 2.0f);
	}

	// endTime shorter than cycleTime * cycles → endTime wins.
	{
		auto actor   = makeActor(group, "Normal", 0, 5, 1.5f);
		auto stepping = makeNormalStepping();
		actor.setStepping(stepping);
		// cycleTime * 5 = 5.0s, endTime = 1.5s → min = 1.5s
		EXPECT_FLOAT_EQ(actor.getRunTime(), 1.5f);
	}

	// endTime longer than cycleTime * cycles → cycles win.
	{
		auto actor   = makeActor(group, "Normal", 0, 2, 10.0f);
		auto stepping = makeNormalStepping();
		actor.setStepping(stepping);
		// cycleTime * 2 = 2.0s, endTime = 10.0s → min = 2.0s
		EXPECT_FLOAT_EQ(actor.getRunTime(), 2.0f);
	}
}

TEST(FrameActorTest, DrawConfig) {
	Actor::setFPS(60);
	Group group("TestGroup", DEFAULT_COLOR);
	auto  actor = makeActor(group, "Fast", 10, 3);
	actor.setStepping(makeNormalStepping());
	actor.restart();
	testing::internal::CaptureStdout();
	actor.drawConfig();
	string output = testing::internal::GetCapturedStdout();
	EXPECT_NE(output.find("Speed: Fast"),       string::npos);
	EXPECT_NE(output.find("Start at Frame:"),   string::npos);
	EXPECT_NE(output.find("Will run for:"),     string::npos);
	EXPECT_NE(output.find("3"),                 string::npos);
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
