/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      ActorTest.cpp
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
#include "animations/Actor.hpp"

using namespace LEDSpicer::Animations;
using namespace LEDSpicer::Devices;
using LEDSpicer::Utilities::Color;

// Global default color for tests.
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

	/**
	 * Gets the current frame counter.
	 * @return
	 */
	static uint8_t getFrame() {
		return frame;
	}

protected:
	void calculateElements() override {
		affectAllElements(true);
	}

	uint16_t getFullFrames() const override {
		return 100; // Arbitrary for testing.
	}

	float getRunTime() const override {
		return secondsToEnd ? secondsToEnd : 0.0f;
	}
};

} // namespace

TEST(ActorTest, Constructor) {
	Group group(
		"TestGroup",
		DEFAULT_COLOR
	);
	StringUMap params = {
		{"type",      "Test"},
		{"group",     "TestGroup"},
		{"filter",    "Normal"},
		{"startTime", "1"},
		{"endTime",   "1"}
	};
	const vector<string> required = {"type", "group", "filter"};
	TestActor actor(params, &group, required);
	EXPECT_EQ(actor.getGroup().getName(), "TestGroup");
	EXPECT_EQ(actor.getNumberOfElements(), 0);
	// Activate actor
	actor.restart();
	// False due to to seconds start.
	EXPECT_FALSE(actor.isRunning());
	sleep_for(std::chrono::milliseconds(1001));
	EXPECT_TRUE(actor.isRunning());
	sleep_for(std::chrono::milliseconds(1001));
	EXPECT_FALSE(actor.isRunning());
	// Test LEDs.
	EXPECT_EQ(actor.getLeds().size(), 0);
}

TEST(ActorTest, ConstructorInvalidParams) {
	Group group(
		"TestGroup",
		DEFAULT_COLOR
	);
	StringUMap params = {
		{"type",  "Test"},
		{"group", "TestGroup"}
		// Missing "filter"
	};
	const vector<string> required = {"type", "group", "filter"};
	EXPECT_THROW(TestActor(params, &group, required), Error);
}

TEST(ActorTest, ChangeElementColor) {
	Group group(
		"TestGroup",
		DEFAULT_COLOR
	);
	uint8_t ledValue = 0;
	Element element(
		"TestElement",
		&ledValue,
		DEFAULT_COLOR,
		0,
		100
	);
	group.linkElement(&element);
	StringUMap params = {
		{"type",   "Test"},
		{"group",  "TestGroup"},
		{"filter", "Normal"}
	};
	const vector<string> required = {"type", "group", "filter"};
	TestActor actor(params, &group, required);
	actor.restart();
	Color color(255, 128, 64);
	actor.changeElementColor(0, color, Color::Filters::Normal, 50);
	// Luminance: 0.301*255 + 0.587*128 + 0.114*64 at 50% = 159
	EXPECT_EQ(ledValue, 159);
}

TEST(ActorTest, ChangeElementsColor) {
	Group group(
		"TestGroup",
		DEFAULT_COLOR
	);
	uint8_t
		ledValue1 = 0,
		ledValue2 = 0;
	Element element1(
		"Element1",
		&ledValue1,
		DEFAULT_COLOR,
		0,
		100
	);
	Element element2(
		"Element2",
		&ledValue2,
		DEFAULT_COLOR,
		0,
		100
	);
	group.linkElement(&element1);
	group.linkElement(&element2);
	StringUMap params = {
		{"type",   "Test"},
		{"group",  "TestGroup"},
		{"filter", "Normal"}
	};
	const vector<string> required = {"type", "group", "filter"};
	TestActor actor(params, &group, required);
	actor.restart();
	Color color(255, 128, 64);
	actor.changeElementsColor(color, Color::Filters::Normal, 50);
	EXPECT_EQ(ledValue1, 159);
	EXPECT_EQ(ledValue2, 159);
}

TEST(ActorTest, TimingAndRestart) {
	Group group(
		"TestGroup",
		DEFAULT_COLOR
	);
	StringUMap params = {
		{"type",      "Test"},
		{"group",     "TestGroup"},
		{"filter",    "Normal"},
		{"startTime", "1"},
		{"endTime",   "2"}
	};
	const vector<string> required = {"type", "group", "filter"};
	TestActor actor(params, &group, required);
	actor.restart();
	// Waiting for startTime (1s)
	EXPECT_FALSE(actor.isRunning());
	// Reset timers
	actor.restart();
	// Still waiting for startTime (1s)
	EXPECT_FALSE(actor.isRunning());
}

TEST(ActorTest, RepeatFunctionality) {
	Group group("TestGroup", DEFAULT_COLOR);
	StringUMap params = {
		{"type",    "Test"},
		{"group",   "TestGroup"},
		{"filter",  "Normal"},
		{"endTime", "1"},
		{"repeat",  "2"}
	};
	const vector<string> required = {"type", "group", "filter"};
	TestActor actor(params, &group, required);
	actor.restart();
	// Wait for startTime (0s, immediate start)
	EXPECT_TRUE(actor.isRunning());
	// Wait for endTime (1s)
	sleep_for(std::chrono::milliseconds(1001));
	// First repeat should trigger restart
	EXPECT_TRUE(actor.isRunning());
	// Wait for endTime again (1s)
	sleep_for(std::chrono::milliseconds(1001));
	// No more repeats, should stop
	EXPECT_FALSE(actor.isRunning());
}

TEST(ActorTest, FPSAndFrame) {
	Actor::setFPS(60);
	EXPECT_EQ(Actor::getFPS(), 60);
	// Frame 1
	TestActor::newFrame();

	EXPECT_EQ(TestActor::getFrame(), 1);
	// advance 60 frames
	for (int i = 0; i < 60; ++i) {
		TestActor::newFrame();
	}
	// Resets after reaching FPS (frame 60)
	EXPECT_EQ(TestActor::getFrame(), 1);
}

TEST(ActorTest, DrawConfig) {
	Group group(
		"TestGroup",
		DEFAULT_COLOR
	);
	StringUMap params = {
		{"type",      "Test"},
		{"group",     "TestGroup"},
		{"filter",    "Combine"},
		{"startTime", "2"},
		{"endTime",   "10"},
		{"repeat",    "3"}
	};
	const vector<string> required = {"type", "group", "filter"};
	TestActor actor(params, &group, required);
	actor.restart(); // Activate actor
	Log::logToStdTerm();
	Log::setLogLevel(LOG_DEBUG);
	testing::internal::CaptureStdout();
	Log::setLogLevel(LOG_ERR);
	actor.drawConfig();
	string output = testing::internal::GetCapturedStdout();
	EXPECT_NE(output.find("Group: TestGroup"),         string::npos);
	EXPECT_NE(output.find("Filter: Combine"),          string::npos);
	EXPECT_NE(output.find("Start After: 2 sec"),       string::npos);
	EXPECT_NE(output.find("Stop After: 10 sec"),       string::npos);
	EXPECT_EQ(output.find("Will repeat for: 3 times"), string::npos);
}

TEST(ActorTest, AffectAllElementsWithMask) {
	Group group(
		"TestGroup",
		DEFAULT_COLOR
	);
	uint8_t
		ledValue1 = 0,
		ledValue2 = 0;
	Element element1(
		"Element1",
		&ledValue1,
		DEFAULT_COLOR,
		0,
		100
	);
	Element element2(
		"Element2",
		&ledValue2,
		DEFAULT_COLOR,
		0,
		100
	);
	group.linkElement(&element1);
	group.linkElement(&element2);
	StringUMap params1 = {
		{"type",   "Test"},
		{"group",  "TestGroup1"},
		{"filter", "Normal"}
	};
	StringUMap params2 = {
		{"type",   "Test"},
		{"group",  "TestGroup2"},
		{"filter", "Mask"}
	};
	const vector<string> required = {"type", "group", "filter"};
	TestActor actor1(params1, &group, required);
	TestActor actor2(params2, &group, required);
	// Test LEDs
	EXPECT_EQ(actor1.getLeds().size(), 2);
	EXPECT_EQ(actor2.getLeds().size(), 2);

	// Activate actors
	actor1.restart();
	actor2.restart();
	Color backgroundColor(200, 100, 50);
	// full color.
	actor1.changeElementsColor(backgroundColor, Color::Filters::Normal, 100);
	// masked.
	actor2.changeElementColor(1, Color(), Color::Filters::Normal, 100);
	actor1.draw();
	actor2.draw();
	EXPECT_EQ(ledValue1, 124);
	EXPECT_EQ(ledValue2, 0);
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
