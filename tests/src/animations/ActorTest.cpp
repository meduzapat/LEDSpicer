/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      ActorTest.cpp
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
#include "animations/Actor.hpp"

using namespace LEDSpicer::Animations;
using namespace LEDSpicer::Devices;
using LEDSpicer::Utilities::Color;

/// Global default color for tests.
const Color DEFAULT_COLOR(255, 255, 255);

namespace LEDSpicer::Animations {

/**
 * TestActor
 * Minimal derived class for testing Actor.
 */
class TestActor : public Actor {

public:

	TestActor(
		StringUMap&           parameters,
		Group* const          group,
		const vector<string>& requiredParameters
	) : Actor(parameters, group, requiredParameters) {}

	/// Returns the current frame counter.
	static uint8_t getFrame() {
		return frame;
	}

	uint32_t getActorCount() const {
		return actorCount;
	}

	uint16_t getRepetitions() const {
		return repeated;
	}

protected:

	void calculateElements() override {
		affectAllElements(true);
	}
};

} // namespace

/**
 * take into consideration that except in a normal nun were isRunning always returns true
 * 1st isRunning after an event always returns false, so even if your actor gets activated it will
 * return false at that frame, the next frame will be true.
 */
TEST(ActorTest, Constructor) {

	Group group("TestGroup", DEFAULT_COLOR);
	StringUMap params {
		{"type",      "Test"},
		{"group",     "TestGroup"},
		{"filter",    "Normal"},
		{"startTime", "1"},
		{"endTime",   "1"}
	};

	const vector<string> required {"type", "group", "filter"};
	TestActor actor(params, &group, required);

	EXPECT_EQ(actor.getGroup().getName(), "TestGroup");
	EXPECT_EQ(actor.getNumberOfElements(), 0);

	Time::setFrameTime();
	actor.restart();

	// Waiting for startTime.
	EXPECT_FALSE(actor.isRunning());
	sleep_for(std::chrono::milliseconds(1001));
	Time::setFrameTime();

	// startTime elapsed, still not running.
	EXPECT_FALSE(actor.isRunning());
	// Now running.
	EXPECT_TRUE(actor.isRunning());
	sleep_for(std::chrono::milliseconds(1001));
	Time::setFrameTime();

	// endTime elapsed, stopped.
	EXPECT_FALSE(actor.isRunning());
}

TEST(ActorTest, ConstructorInvalidParams) {

	Group group("TestGroup", DEFAULT_COLOR);
	StringUMap params {
		{"type",  "Test"},
		{"group", "TestGroup"}
		// Missing "filter"
	};
	const vector<string> required {"type", "group", "filter"};
	EXPECT_THROW(TestActor(params, &group, required), Error);
}

TEST(ActorTest, NormalRun) {

	Group group("TestGroup", DEFAULT_COLOR);
	StringUMap params {
		{"type",   "Test"},
		{"group",  "TestGroup"},
		{"filter", "Normal"}
	};
	const vector<string> required {"type", "group", "filter"};
	TestActor actor(params, &group, required);
	actor.restart();
	// No timers — always running.
	EXPECT_TRUE(actor.isRunning());
	sleep_for(std::chrono::milliseconds(500));
	EXPECT_TRUE(actor.isRunning());
}

TEST(ActorTest, StartTime) {

	Group group("TestGroup", DEFAULT_COLOR);
	StringUMap params {
		{"type",      "Test"},
		{"group",     "TestGroup"},
		{"filter",    "Normal"},
		{"startTime", "1"}
	};
	const vector<string> required {"type", "group", "filter"};
	TestActor actor(params, &group, required);
	Time::setFrameTime();
	actor.restart();

	// Waiting for startTime.
	EXPECT_FALSE(actor.isRunning());

	// restart() re-arms startTime.
	actor.restart();
	EXPECT_FALSE(actor.isRunning());

	sleep_for(std::chrono::milliseconds(1001));
	Time::setFrameTime();

	// startTime elapsed → normal run.
	EXPECT_FALSE(actor.isRunning());
	// Now running.
	EXPECT_TRUE(actor.isRunning());
	EXPECT_TRUE(actor.isRunning());
}

TEST(ActorTest, EndTime) {

	Group group("TestGroup", DEFAULT_COLOR);
	StringUMap params {
		{"type",    "Test"},
		{"group",   "TestGroup"},
		{"filter",  "Normal"},
		{"endTime", "1"}
	};
	const vector<string> required {"type", "group", "filter"};
	TestActor actor(params, &group, required);
	Time::setFrameTime();
	actor.restart();

	// Running until endTime.
	EXPECT_TRUE(actor.isRunning());
	sleep_for(std::chrono::milliseconds(1001));
	Time::setFrameTime();

	// endTime elapsed — permanently off (no repeat, no restartTime).
	EXPECT_FALSE(actor.isRunning());
	EXPECT_FALSE(actor.isRunning());
}

TEST(ActorTest, RepeatOnce) {

	Group group("TestGroup", DEFAULT_COLOR);
	StringUMap params {
		{"type",    "Test"},
		{"group",   "TestGroup"},
		{"filter",  "Normal"},
		{"endTime", "1"},
		{"repeat",  "1"}
	};
	const vector<string> required {"type", "group", "filter"};
	TestActor actor(params, &group, required);
	Time::setFrameTime();
	actor.restart();


	// Run 1.
	EXPECT_TRUE(actor.isRunning());
	EXPECT_EQ(actor.getRepetitions(), 0);
	sleep_for(std::chrono::milliseconds(1001));
	Time::setFrameTime();

	// endTime → repeat 1 triggers immediate restart.
	EXPECT_FALSE(actor.isRunning());
	EXPECT_EQ(actor.getRepetitions(), 1);

	// Run 2, wait for the end time to kick in.
	EXPECT_TRUE(actor.isRunning());
	sleep_for(std::chrono::milliseconds(1001));
	Time::setFrameTime();

	// Repeats exhausted — permanently off.
	EXPECT_FALSE(actor.isRunning());
	EXPECT_FALSE(actor.isRunning());
}

TEST(ActorTest, RepeatTwice) {

	Group group("TestGroup", DEFAULT_COLOR);
	StringUMap params {
		{"type",    "Test"},
		{"group",   "TestGroup"},
		{"filter",  "Normal"},
		{"endTime", "1"},
		{"repeat",  "2"}
	};
	const vector<string> required {"type", "group", "filter"};
	TestActor actor(params, &group, required);
	Time::setFrameTime();
	actor.restart();

	// Run 1.
	EXPECT_TRUE(actor.isRunning());
	EXPECT_EQ(actor.getRepetitions(), 0);
	sleep_for(std::chrono::milliseconds(1001));
	Time::setFrameTime();

	// endTime → repeat 1.
	EXPECT_FALSE(actor.isRunning());
	EXPECT_EQ(actor.getRepetitions(), 1);
	EXPECT_TRUE(actor.isRunning());
	sleep_for(std::chrono::milliseconds(1001));
	Time::setFrameTime();

	// endTime → repeat 2.
	EXPECT_FALSE(actor.isRunning());
	EXPECT_EQ(actor.getRepetitions(), 2);
	EXPECT_TRUE(actor.isRunning());
	sleep_for(std::chrono::milliseconds(1001));
	Time::setFrameTime();

	// Repeats exhausted — permanently off.
	EXPECT_FALSE(actor.isRunning());
	EXPECT_FALSE(actor.isRunning());
}

TEST(ActorTest, RepeatInfinite) {

	Group group("TestGroup", DEFAULT_COLOR);
	StringUMap params {
		{"type",    "Test"},
		{"group",   "TestGroup"},
		{"filter",  "Normal"},
		{"endTime", "1"},
		{"repeat",  "-1"}
	};
	const vector<string> required {"type", "group", "filter"};
	TestActor actor(params, &group, required);
	Time::setFrameTime();
	actor.restart();

	// Verify 3 full cycles — repeats indefinitely.
	for (uint8_t i = 0; i < 3; ++i) {
		EXPECT_TRUE(actor.isRunning());
		// never moves.
		EXPECT_EQ(actor.getRepetitions(), 0);
		sleep_for(std::chrono::milliseconds(1001));
		Time::setFrameTime();
		EXPECT_FALSE(actor.isRunning());
		EXPECT_TRUE(actor.isRunning());
	}
}

TEST(ActorTest, RestartTime) {

	Group group("TestGroup", DEFAULT_COLOR);
	StringUMap params {
		{"type",        "Test"},
		{"group",       "TestGroup"},
		{"filter",      "Normal"},
		{"endTime",     "1"},
		{"restartTime", "1"}
	};
	const vector<string> required {"type", "group", "filter"};
	TestActor actor(params, &group, required);
	Time::setFrameTime();
	actor.restart();

	// Cycle 1: running.
	EXPECT_TRUE(actor.isRunning());
	sleep_for(std::chrono::milliseconds(1001));
	Time::setFrameTime();

	// endTime fired → paused (restartTime ticking).
	EXPECT_FALSE(actor.isRunning());
	sleep_for(std::chrono::milliseconds(1001));
	Time::setFrameTime();

	// restartTime fired → kicks restart on next call.
	EXPECT_FALSE(actor.isRunning());
	EXPECT_TRUE(actor.isRunning());
	sleep_for(std::chrono::milliseconds(1001));
	Time::setFrameTime();

	// Cycle 2: endTime again.
	EXPECT_FALSE(actor.isRunning());
	sleep_for(std::chrono::milliseconds(1001));
	Time::setFrameTime();

	// Cycle 3: infinite, keeps going.
	EXPECT_FALSE(actor.isRunning());
	EXPECT_TRUE(actor.isRunning());
}

TEST(ActorTest, RestartTimeWithoutEndTimeIsNoOp) {

	Group group("TestGroup", DEFAULT_COLOR);
	StringUMap params {
		{"type",        "Test"},
		{"group",       "TestGroup"},
		{"filter",      "Normal"},
		{"restartTime", "1"}
	};
	const vector<string> required {"type", "group", "filter"};
	testing::internal::CaptureStdout();
	TestActor actor(params, &group, required);
	string output = testing::internal::GetCapturedStdout();
	EXPECT_NE(output.find("restartTime has no effect without endTime"), string::npos);
	actor.restart();
	EXPECT_TRUE(actor.isRunning());
	EXPECT_TRUE(actor.isRunning());
}

TEST(ActorTest, GetRunTime) {

	Group group("TestGroup", DEFAULT_COLOR);
	const vector<string> required {"type", "group", "filter"};

	// No endTime → 0 (∞).
	{
		StringUMap params {{"type", "Test"}, {"group", "TestGroup"}, {"filter", "Normal"}};
		TestActor actor(params, &group, required);
		EXPECT_FLOAT_EQ(actor.getRunTime(), 0.0f);
	}

	// endTime only, no repeat → 1 run.
	{
		StringUMap params {{"type", "Test"}, {"group", "TestGroup"}, {"filter", "Normal"}, {"endTime", "4"}};
		TestActor actor(params, &group, required);
		EXPECT_FLOAT_EQ(actor.getRunTime(), 4.0f);
	}

	// endTime + repeat=1 → 2 total runs.
	{
		StringUMap params {{"type", "Test"}, {"group", "TestGroup"}, {"filter", "Normal"}, {"endTime", "4"}, {"repeat", "1"}};
		TestActor actor(params, &group, required);
		EXPECT_FLOAT_EQ(actor.getRunTime(), 8.0f);
	}

	// endTime + repeat=2 → 3 total runs.
	{
		StringUMap params {{"type", "Test"}, {"group", "TestGroup"}, {"filter", "Normal"}, {"endTime", "4"}, {"repeat", "2"}};
		TestActor actor(params, &group, required);
		EXPECT_FLOAT_EQ(actor.getRunTime(), 12.0f);
	}

	// repeat=-1 (infinite) → 0 (∞).
	{
		StringUMap params {{"type", "Test"}, {"group", "TestGroup"}, {"filter", "Normal"}, {"endTime", "4"}, {"repeat", "-1"}};
		TestActor actor(params, &group, required);
		EXPECT_FLOAT_EQ(actor.getRunTime(), 0.0f);
	}
}

TEST(ActorTest, FPSAndFrame) {
	Actor::setFPS(60);
	EXPECT_EQ(Actor::getFPS(), 60);
	TestActor::newFrame();
	EXPECT_EQ(TestActor::getFrame(), 1);
	// Advance a full second.
	for (uint8_t i = 0; i < 60; ++i)
		TestActor::newFrame();
	// Resets after reaching FPS.
	EXPECT_EQ(TestActor::getFrame(), 1);
}

TEST(ActorTest, ChangeElementColor) {

	Group group("TestGroup", DEFAULT_COLOR);
	uint8_t  ledValue = 0;
	Element  element("TestElement", &ledValue, DEFAULT_COLOR, 0, 100);
	group.linkElement(&element);
	StringUMap params {
		{"type",   "Test"},
		{"group",  "TestGroup"},
		{"filter", "Normal"}
	};
	const vector<string> required {"type", "group", "filter"};
	TestActor actor(params, &group, required);
	actor.restart();
	Color color(255, 128, 64);
	actor.changeElementColor(0, color, Color::Filters::Normal, 50);
	// Luminance at 50%: 0.301*255 + 0.587*128 + 0.114*64 = 159
	EXPECT_EQ(ledValue, 159);
}

TEST(ActorTest, ChangeElementsColor) {

	Group group("TestGroup", DEFAULT_COLOR);
	uint8_t
		ledValue1 = 0,
		ledValue2 = 0;
	Element
		element1("Element1", &ledValue1, DEFAULT_COLOR, 0, 100),
		element2("Element2", &ledValue2, DEFAULT_COLOR, 0, 100);
	group.linkElement(&element1);
	group.linkElement(&element2);
	StringUMap params {
		{"type",   "Test"},
		{"group",  "TestGroup"},
		{"filter", "Normal"}
	};
	const vector<string> required {"type", "group", "filter"};
	TestActor actor(params, &group, required);
	actor.restart();
	Color color(255, 128, 64);
	actor.changeElementsColor(color, Color::Filters::Normal, 50);
	EXPECT_EQ(ledValue1, 159);
	EXPECT_EQ(ledValue2, 159);
}

TEST(ActorTest, AffectAllElementsWithMask) {

	Group group("TestGroup", DEFAULT_COLOR);
	uint8_t
		ledValue1 = 0,
		ledValue2 = 0;
	Element
		element1("Element1", &ledValue1, DEFAULT_COLOR, 0, 100),
		element2("Element2", &ledValue2, DEFAULT_COLOR, 0, 100);
	group.linkElement(&element1);
	group.linkElement(&element2);
	StringUMap
		params1 {{"type", "Test"}, {"group", "TestGroup1"}, {"filter", "Normal"}},
		params2 {{"type", "Test"}, {"group", "TestGroup2"}, {"filter", "Mask"}};
	const vector<string> required {"type", "group", "filter"};
	TestActor
		actor1(params1, &group, required),
		actor2(params2, &group, required);
	EXPECT_EQ(actor1.getLeds().size(), 2);
	EXPECT_EQ(actor2.getLeds().size(), 2);

	actor1.restart();
	actor2.restart();
	Color backgroundColor(200, 100, 50);
	actor1.changeElementsColor(backgroundColor, Color::Filters::Normal, 100);
	actor2.changeElementColor(1, Color(), Color::Filters::Normal, 100);
	actor1.draw();
	actor2.draw();
	EXPECT_EQ(ledValue1, 124);
	EXPECT_EQ(ledValue2, 0);
}

TEST(ActorTest, DrawConfig) {

	Group group("TestGroup", DEFAULT_COLOR);
	StringUMap params {
		{"type",      "Test"},
		{"group",     "TestGroup"},
		{"filter",    "Combine"},
		{"startTime", "2"},
		{"endTime",   "10"},
		{"repeat",    "3"}
	};
	const vector<string> required {"type", "group", "filter"};
	TestActor actor(params, &group, required);
	actor.restart();
	testing::internal::CaptureStdout();
	actor.drawConfig();
	const string
		output  {testing::internal::GetCapturedStdout()},
		actorId {std::to_string(actor.getActorCount() -1)};
	const auto sec10 {output.find("10.00 sec")};
	EXPECT_NE(output.find("Actor ID:"),        string::npos);
	EXPECT_NE(output.find(actorId),            string::npos);
	EXPECT_NE(output.find("Group: TestGroup"), string::npos);
	EXPECT_NE(output.find("Filter: Combine"),  string::npos);
	EXPECT_NE(output.find("Start After:"),     string::npos);
	EXPECT_NE(output.find("2.00 sec"),         string::npos);
	EXPECT_NE(output.find("Stop After:"),      string::npos);
	EXPECT_NE(sec10,                           string::npos); // 1st find
	EXPECT_NE(output.find("Will repeat:"),     string::npos);
	EXPECT_NE(output.find("3 times"),          string::npos);
	EXPECT_NE(output.find("Running time:"),    string::npos);
	EXPECT_NE(output.find("10.00 sec", sec10), string::npos); // 2nd find
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
