/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      StepActorTest.cpp
 * @since     Sep 7, 2025
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
#include "animations/StepActor.hpp"

namespace LEDSpicer::Animations {

/**
 * TestStepActor
 * Minimal derived class for testing StepActor.
 */
class TestStepActor: public StepActor {

public:

	TestStepActor(
		StringUMap& parameters,
		Group* const group,
		const vector<string>& requiredParameters
	) : StepActor(parameters, group, requiredParameters) {}

	/**
	 * @param stepping the stepping to test.
	 */
	void setStepping(FrameActor::Stepping stepping) {
		this->stepping = stepping;
	}

	/**
	 * @return the step percent.
	 */
	float getStepPercent() const {
		return stepPercent;
	}

	/**
	 * Exposes changeFrameElement for testing.
	 * @param index
	 * @param color
	 * @param direction
	 */
	void testChangeFrameElement(uint16_t index, const Color& color, Directions direction) {
		changeFrameElement(index, color, direction);
	}

	/**
	 * Exposes changeFrameElement for testing.
	 * @param color
	 * @param direction
	 */
	void testChangeFrameElement(const Color& color, Directions direction) {
		changeFrameElement(color, direction);
	}

	/**
	 * Exposes changeFrameElement for testing.
	 * @param color
	 * @param fade
	 * @param direction
	 */
	void testChangeFrameElement(const Color& color, bool fade, Directions direction) {
		changeFrameElement(color, fade, direction);
	}

	/**
	 * Exposes changeFrameElement for testing.
	 * @param color
	 * @param colorNext
	 * @param direction
	 */
	void testChangeFrameElement(const Color& color, const Color& colorNext, Directions direction) {
		changeFrameElement(color, colorNext, direction);
	}

protected:

	void calculateElements() override {
		affectAllElements(true);
	}
};

} // namespace

using namespace LEDSpicer::Animations;
using namespace LEDSpicer::Devices;
using LEDSpicer::Utilities::Color;
using LEDSpicer::Utilities::Direction;

const Color DEFAULT_COLOR(255, 255, 255);


TEST(StepActorTest, CalculateStepPercent) {
	Group group("TestGroup", DEFAULT_COLOR);
	StringUMap params = {
		{"type",      "Test"},
		{"group",     "TestGroup"},
		{"filter",    "Normal"},
		{"speed",     "Normal"},
		{"direction", "Forward"}
	};
	FrameActor::Stepping stepping;
	const vector<string> required = {"type", "group", "filter", "speed", "direction"};
	TestStepActor actor(params, &group, required);
	actor.calculateStepPercent();
	EXPECT_FLOAT_EQ(actor.getStepPercent(), 0.0f);

	stepping.steps = 10;
	stepping.step  = 5;
	actor.setStepping(stepping);
	actor.calculateStepPercent();
	EXPECT_FLOAT_EQ(actor.getStepPercent(), 10.0f); // 100 / 10 = 10%
}

TEST(StepActorTest, ChangeFrameElementIndexColorDirection) {
	Group group("TestGroup", DEFAULT_COLOR);
	uint8_t ledValue = 0;
	Element element("Element", &ledValue, DEFAULT_COLOR, 0, 100);
	group.linkElement(&element);
	StringUMap params = {
		{"type",      "Test"},
		{"group",     "TestGroup"},
		{"filter",    "Normal"},
		{"speed",     "Normal"},
		{"direction", "Forward"}
	};
	const vector<string> required = {"type", "group", "filter", "speed", "direction"};
	TestStepActor actor(params, &group, required);
	FrameActor::Stepping stepping;
	stepping.steps = 10;
	stepping.step  = 5;
	actor.setStepping(stepping);
	actor.calculateStepPercent(); // 10%

	Color color(255, 128, 64);

	// Forward: percent = 50, faded (127,64,32), mono=79
	actor.testChangeFrameElement(0, color, Direction::Directions::Forward);
	EXPECT_EQ(ledValue, 79);

	// Backward: percent = 100 - 50 = 50 (same as above)
	ledValue = 0;
	actor.testChangeFrameElement(0, color, Direction::Directions::Backward);
	EXPECT_EQ(ledValue, 79);
}

TEST(StepActorTest, ChangeFrameElementColorDirection) {
	Group group("TestGroup", DEFAULT_COLOR);
	uint8_t ledValue = 0;
	Element element("Element", &ledValue, DEFAULT_COLOR, 0, 100);
	group.linkElement(&element);
	StringUMap params = {
		{"type",      "Test"},
		{"group",     "TestGroup"},
		{"filter",    "Normal"},
		{"speed",     "Normal"},
		{"direction", "Forward"}
	};
	const vector<string> required = {"type", "group", "filter", "speed", "direction"};
	TestStepActor actor(params, &group, required);
	FrameActor::Stepping stepping;
	stepping.frame = 0;
	stepping.steps = 5;
	stepping.step  = 2;
	actor.setStepping(stepping);
	actor.calculateStepPercent(); // 20%

	Color color(200, 100, 50);

	// Forward: percent = 40, faded (80,40,20), mono=49
	actor.testChangeFrameElement(color, Direction::Directions::Forward);
	EXPECT_EQ(ledValue, 49);

	// Backward: percent = 100 - 40 = 60, faded (120,60,30), mono=74
	ledValue = 0;
	actor.testChangeFrameElement(color, Direction::Directions::Backward);
	EXPECT_EQ(ledValue, 74);
}

TEST(StepActorTest, ChangeFrameElementColorFadeDirection) {
	Group group("TestGroup", DEFAULT_COLOR);
	uint8_t
		ledValue1 = 0,
		ledValue2 = 0;
	Element
		element1("Element1", &ledValue1, DEFAULT_COLOR, 0, 100),
		element2("Element2", &ledValue2, DEFAULT_COLOR, 0, 100);
	group.linkElement(&element1);
	group.linkElement(&element2);
	StringUMap params = {
		{"type",      "Test"},
		{"group",     "TestGroup"},
		{"filter",    "Normal"},
		{"speed",     "Normal"},
		{"direction", "Forward"}
	};
	const vector<string> required = {"type", "group", "filter", "speed", "direction"};
	TestStepActor actor(params, &group, required);
	FrameActor::Stepping stepping;
	stepping.frames = 2;
	stepping.frame  = 0;
	stepping.steps  = 4;
	stepping.step   = 1;
	actor.setStepping(stepping);
	actor.calculateStepPercent(); // 25%

	Color color(255, 255, 255);

	// Fade true, Forward
	actor.testChangeFrameElement(color, true, Direction::Directions::Forward);
	// 255 * 0.75 = 191, mono=191
	EXPECT_EQ(ledValue1, 191);
	// 255 * 0.25 = 63, mono=63
	EXPECT_EQ(ledValue2, 63);

	// Fade false
	ledValue1 = 0;
	ledValue2 = 0;
	actor.testChangeFrameElement(color, false, Direction::Directions::Forward);
	EXPECT_EQ(ledValue1, 255);
	EXPECT_EQ(ledValue2, 0);

	// Boundary: at end, forward
	stepping.frame = 1;
	actor.setStepping(stepping);
	ledValue1 = 0;
	ledValue2 = 0;
	actor.testChangeFrameElement(color, true, Direction::Directions::Forward);
	EXPECT_EQ(ledValue1, 63);
	EXPECT_EQ(ledValue2, 191);
}

TEST(StepActorTest, ChangeFrameElementColorColorDirection) {
	Group group("TestGroup", DEFAULT_COLOR);
	uint8_t
		ledValue1 = 0,
		ledValue2 = 0;
	Element
		element1("Element1", &ledValue1, DEFAULT_COLOR, 0, 100),
		element2("Element2", &ledValue2, DEFAULT_COLOR, 0, 100);
	group.linkElement(&element1);
	group.linkElement(&element2);
	StringUMap params = {
		{"type",      "Test"},
		{"group",     "TestGroup"},
		{"filter",    "Normal"},
		{"speed",     "Normal"},
		{"direction", "Forward"}
	};
	const vector<string> required = {"type", "group", "filter", "speed", "direction"};
	TestStepActor actor(params, &group, required);
	FrameActor::Stepping stepping;
	stepping.frames = 2;
	stepping.frame  = 0;
	stepping.steps  = 5;
	stepping.step   = 2;
	actor.setStepping(stepping);
	actor.calculateStepPercent(); // 20%

	Color color(255, 0, 0);      // Current: red
	Color colorNext(0, 255, 0);  // Next: green

	actor.testChangeFrameElement(color, colorNext, Direction::Directions::Forward);
	// Current: transition (102,153,0), mono=120
	EXPECT_EQ(ledValue1, 120);
	// Next: fade(40) (0,102,0), mono=59
	EXPECT_EQ(ledValue2, 59);
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
