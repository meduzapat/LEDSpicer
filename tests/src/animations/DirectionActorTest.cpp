/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DirectionActorTest.cpp
 * @since     Sep 3, 2025
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
#include "animations/DirectionActor.hpp"

using namespace LEDSpicer::Animations;
using namespace LEDSpicer::Devices;
using LEDSpicer::Utilities::Color;
using LEDSpicer::Utilities::Direction;

// Global default color for tests.
const Color DEFAULT_COLOR(255, 255, 255);

namespace LEDSpicer::Animations {

/**
 * TestDirectionActor
 * Minimal derived class for testing DirectionActor.
 */
class TestDirectionActor : public DirectionActor {
public:
	TestDirectionActor(
		StringUMap&           parameters,
		Group* const          group,
		const vector<string>& requiredParameters
	) : DirectionActor(parameters, group, requiredParameters) {}

	/**
	 * @param stepping the stepping to test.
	 */
	void setStepping(FrameActor::Stepping stepping) {
		this->stepping = stepping;
	}

	/**
	 * Gets the current direction state.
	 * @return
	 */
	Direction getCurrentDirection() const {
		return cDirection;
	}

	/**
	 * Gets the current frame.
	 * @return
	 */
	uint16_t getCurrentFrame() const {
		return stepping.frame;
	}

protected:
	void calculateElements() override {
		affectAllElements(true);
	}
};

} // namespace

TEST(DirectionActorTest, ConstructorForward) {
	Group group("TestGroup", DEFAULT_COLOR);
	StringUMap params = {
		{"type",      "Test"},
		{"group",     "TestGroup"},
		{"filter",    "Normal"},
		{"speed",     "Normal"},
		{"direction", "Forward"}
	};
	const vector<string> required = {"type", "group", "filter", "speed", "direction"};
	Actor::setFPS(60);
	TestDirectionActor actor(params, &group, required);
	FrameActor::Stepping stepping;
	stepping.frames = FrameActor::calculateFramesBySpeed(Speed::Speeds::Normal);
	actor.setStepping(stepping);
	EXPECT_TRUE(actor.isForward());
	EXPECT_FALSE(actor.isBackward());
	EXPECT_FALSE(actor.isBouncing());
	EXPECT_EQ(actor.getCurrentFrame(), 0);
	EXPECT_EQ(actor.getFullFrames(), 60);
}

TEST(DirectionActorTest, ConstructorBackward) {
	Group group("TestGroup", DEFAULT_COLOR);
	StringUMap params = {
		{"type",      "Test"},
		{"group",     "TestGroup"},
		{"filter",    "Normal"},
		{"speed",     "Normal"},
		{"direction", "Backward"}
	};
	const vector<string> required = {"type", "group", "filter", "speed", "direction"};
	TestDirectionActor actor(params, &group, required);
	FrameActor::Stepping stepping;
	stepping.frames = FrameActor::calculateFramesBySpeed(Speed::Speeds::Normal);
	actor.setStepping(stepping);
	actor.restart();
	EXPECT_FALSE(actor.isForward());
	EXPECT_TRUE(actor.isBackward());
	EXPECT_FALSE(actor.isBouncing());
	EXPECT_EQ(actor.getCurrentFrame(), 59);
	EXPECT_EQ(actor.getFullFrames(), 60);
}

TEST(DirectionActorTest, ConstructorBouncer) {
	Group group("TestGroup", DEFAULT_COLOR);
	StringUMap params = {
		{"type",      "Test"},
		{"group",     "TestGroup"},
		{"filter",    "Normal"},
		{"speed",     "Normal"},
		{"direction", "Forward"},
		{"bouncer",   "True"}
	};
	const vector<string> required = {"type", "group", "filter", "speed", "direction"};
	TestDirectionActor actor(params, &group, required);
	FrameActor::Stepping stepping;
	stepping.frames = FrameActor::calculateFramesBySpeed(Speed::Speeds::Normal);
	actor.setStepping(stepping);
	actor.restart();
	EXPECT_TRUE(actor.isForward());
	EXPECT_FALSE(actor.isBackward());
	EXPECT_TRUE(actor.isBouncer());
	EXPECT_EQ(actor.getCurrentFrame(), 0);
	// Doubled due to bouncer
	EXPECT_EQ(actor.getFullFrames(), 120);
}

TEST(DirectionActorTest, ConstructorInvalidParams) {
	Group group("TestGroup", DEFAULT_COLOR);
	StringUMap params = {
		{"type",   "Test"},
		{"group",  "TestGroup"},
		{"filter", "Normal"},
		{"speed",  "Normal"}
		// Missing "direction"
	};
	const vector<string> required = {"type", "group", "filter", "speed"};
	EXPECT_THROW(TestDirectionActor(params, &group, required), Error);
}

TEST(DirectionActorTest, StartOfCycle) {
	Actor::setFPS(60);
	Group group("TestGroup", DEFAULT_COLOR);
	const vector<string> required = {"type", "group", "filter", "speed", "direction"};

	// Forward direction
	StringUMap paramsForward = {
		{"type",      "Test"},
		{"group",     "TestGroup"},
		{"filter",    "Normal"},
		{"speed",     "Normal"},
		{"direction", "Forward"}
	};
	TestDirectionActor actorForward(paramsForward, &group, required);
	FrameActor::Stepping stepping;
	stepping.frames = FrameActor::calculateFramesBySpeed(Speed::Speeds::Normal);
	stepping.steps  = 2; // Set steps to 2 to test step progression
	actorForward.setStepping(stepping);
	actorForward.restart();
	EXPECT_TRUE(actorForward.isStartOfCycle()); // frame == 0, step == 0
	EXPECT_TRUE(actorForward.isFirstFrame());
	EXPECT_TRUE(actorForward.getCurrentDirection().isForward());
	actorForward.draw();
	EXPECT_FALSE(actorForward.isStartOfCycle()); // frame == 0, step == 1
	EXPECT_TRUE(actorForward.isFirstFrame());
	EXPECT_TRUE(actorForward.getCurrentDirection().isForward());

	// Backward direction
	StringUMap paramsBackward = {
		{"type",      "Test"},
		{"group",     "TestGroup"},
		{"filter",    "Normal"},
		{"speed",     "Normal"},
		{"direction", "Backward"}
	};
	TestDirectionActor actorBackward(paramsBackward, &group, required);
	actorBackward.setStepping(stepping);
	actorBackward.restart();
	EXPECT_TRUE(actorBackward.isStartOfCycle()); // frame == 59, step == 0
	EXPECT_TRUE(actorBackward.isFirstFrame());
	EXPECT_TRUE(actorBackward.getCurrentDirection().isBackward());
	actorBackward.draw();
	EXPECT_FALSE(actorBackward.isStartOfCycle()); // frame == 59, step == 1
	EXPECT_TRUE(actorBackward.isFirstFrame());
	EXPECT_TRUE(actorBackward.getCurrentDirection().isBackward());

	// Bouncer mode (Forward start)
	StringUMap paramsBouncer = {
		{"type",      "Test"},
		{"group",     "TestGroup"},
		{"filter",    "Normal"},
		{"speed",     "Normal"},
		{"direction", "Forward"},
		{"bouncer",   "True"}
	};
	TestDirectionActor actorBouncer(paramsBouncer, &group, required);
	actorBouncer.setStepping(stepping);
	actorBouncer.restart();
	EXPECT_TRUE(actorBouncer.isStartOfCycle()); // frame == 0, step == 0
	EXPECT_TRUE(actorBouncer.isFirstFrame());
	EXPECT_TRUE(actorBouncer.getCurrentDirection().isForward());
	// Advance to last frame
	for (int i = 0; i < 59 * 2; ++i) actorBouncer.draw(); // 59 frames * 2 steps
	EXPECT_FALSE(actorBouncer.isStartOfCycle()); // frame == 59, step == 0
	EXPECT_TRUE(actorBouncer.isLastFrame());
	EXPECT_TRUE(actorBouncer.getCurrentDirection().isForward());
	actorBouncer.draw(); // Bounce: frame == 59, step == 1
	actorBouncer.draw(); // frame == 59, step == 0, direction backward
	EXPECT_TRUE(actorBouncer.isStartOfCycle()); // frame == 59, step == 0, backward
	EXPECT_TRUE(actorBouncer.isFirstFrame());
	EXPECT_TRUE(actorBouncer.getCurrentDirection().isBackward());
}

TEST(DirectionActorTest, ForwardFrameAdvance) {
	Group group("TestGroup", DEFAULT_COLOR);
	StringUMap params = {
		{"type",      "Test"},
		{"group",     "TestGroup"},
		{"filter",    "Normal"},
		{"speed",     "Normal"},
		{"direction", "Forward"}
	};
	const vector<string> required = {"type", "group", "filter", "speed", "direction"};
	TestDirectionActor actor(params, &group, required);
	FrameActor::Stepping stepping;
	stepping.frames = FrameActor::calculateFramesBySpeed(Speed::Speeds::Normal);
	actor.setStepping(stepping);
	actor.restart();
	EXPECT_EQ(actor.getCurrentFrame(), 0);
	actor.draw(); // Advances frame
	EXPECT_EQ(actor.getCurrentFrame(), 1);
	for (int i = 0; i < 57; ++i) actor.draw();
	EXPECT_EQ(actor.getCurrentFrame(), 58);
	EXPECT_FALSE(actor.isLastFrame());
	actor.draw();
	EXPECT_TRUE(actor.isLastFrame());
	actor.draw();
	// Wraps around
	EXPECT_EQ(actor.getCurrentFrame(), 0);
}

TEST(DirectionActorTest, BackwardFrameAdvance) {
	Group group("TestGroup", DEFAULT_COLOR);
	StringUMap params = {
		{"type",      "Test"},
		{"group",     "TestGroup"},
		{"filter",    "Normal"},
		{"speed",     "Normal"},
		{"direction", "Backward"}
	};
	const vector<string> required = {"type", "group", "filter", "speed", "direction"};
	TestDirectionActor actor(params, &group, required);
	FrameActor::Stepping stepping;
	stepping.frames = FrameActor::calculateFramesBySpeed(Speed::Speeds::Normal);
	actor.setStepping(stepping);
	actor.restart();
	EXPECT_EQ(actor.getCurrentFrame(), 59);
	// Advances frame
	actor.draw();
	EXPECT_EQ(actor.getCurrentFrame(), 58);
	// advance a full cycle.
	for (int i = 0; i < 57; ++i) actor.draw();
	// one before the last frame.
	EXPECT_EQ(actor.getCurrentFrame(), 1);
	EXPECT_FALSE(actor.isLastFrame());
	// Last.
	actor.draw();
	EXPECT_TRUE(actor.isLastFrame());
	actor.draw();
	// Wraps around
	EXPECT_EQ(actor.getCurrentFrame(), 59);
}

TEST(DirectionActorTest, BouncerFrameAdvance) {
	Group group("TestGroup", DEFAULT_COLOR);
	StringUMap params = {
		{"type",      "Test"},
		{"group",     "TestGroup"},
		{"filter",    "Normal"},
		{"speed",     "Normal"},
		{"direction", "Forward"},
		{"bouncer",   "True"}
	};
	const vector<string> required = {"type", "group", "filter", "speed", "direction"};
	TestDirectionActor actor(params, &group, required);
	FrameActor::Stepping stepping;
	stepping.frames = FrameActor::calculateFramesBySpeed(Speed::Speeds::Normal);
	actor.setStepping(stepping);
	actor.restart();
	EXPECT_TRUE(actor.getCurrentDirection().isForward());
	EXPECT_EQ(actor.getCurrentFrame(), 0);
	// Move to last frame
	for (int i = 0; i < 59; ++i) actor.draw();
	EXPECT_EQ(actor.getCurrentFrame(), 59);
	EXPECT_TRUE(actor.isLastFrame());
	actor.draw();
	// Direction reverses
	EXPECT_FALSE(actor.getCurrentDirection().isForward());
	// Stays at 60 due to bounce
	EXPECT_EQ(actor.getCurrentFrame(), 59);
	actor.draw();
	// Moves backward
	EXPECT_EQ(actor.getCurrentFrame(), 58);
}

TEST(DirectionActorTest, CalculateNextOf) {
	Direction direction(Direction::Directions::Forward, true); // Bouncer
	Direction currentDirection(Direction::Directions::Forward);
	uint16_t index;

	// Forward, non-boundary
	index = DirectionActor::calculateNextOf(currentDirection, 5, direction, 9);
	EXPECT_EQ(index, 6);
	EXPECT_TRUE(currentDirection.isForward());
	EXPECT_FALSE(currentDirection.isBouncer());

	// Forward, at end, bouncer
	index = DirectionActor::calculateNextOf(currentDirection, 9, direction, 9);
	EXPECT_EQ(index, 9);
	// Reverses.
	EXPECT_FALSE(currentDirection.isForward());
	// Is bouncing.
	EXPECT_TRUE(currentDirection.isBouncer());

	// Backward, non-boundary
	index = DirectionActor::calculateNextOf(currentDirection, 5, direction, 9);
	EXPECT_EQ(index, 4);
	EXPECT_FALSE(currentDirection.isForward());

	// Backward, at start, bouncer
	index = DirectionActor::calculateNextOf(currentDirection, 0, direction, 9);
	EXPECT_EQ(index, 0);
	EXPECT_TRUE(currentDirection.isForward()); // Reverses
}

TEST(DirectionActorTest, NextOf) {
	Direction direction(Direction::Directions::Forward, true); // Bouncer
	Direction currentDirection(Direction::Directions::Forward);
	uint16_t index;

	// Forward, non-boundary
	index = DirectionActor::nextOf(currentDirection, 5, direction, 9);
	EXPECT_EQ(index, 6);

	// Forward, at end, bouncer
	index = DirectionActor::nextOf(currentDirection, 9, direction, 9);
	EXPECT_EQ(index, 9);

	// Backward, non-boundary
	currentDirection = Direction::Directions::Backward;
	index = DirectionActor::nextOf(currentDirection, 5, direction, 9);
	EXPECT_EQ(index, 4);

	// Backward, at start, bouncer
	index = DirectionActor::nextOf(currentDirection, 0, direction, 9);
	EXPECT_EQ(index, 0);
}

TEST(DirectionActorTest, DrawConfig) {
	Group group("TestGroup", DEFAULT_COLOR);
	StringUMap params = {
		{"type",      "Test"},
		{"group",     "TestGroup"},
		{"filter",    "Normal"},
		{"speed",     "Normal"},
		{"direction", "Forward"},
		{"bouncer",   "True"}
	};
	const vector<string> required = {"type", "group", "filter", "speed", "direction"};
	TestDirectionActor actor(params, &group, required);
	actor.restart();
	Log::logToStdTerm();
	Log::setLogLevel(LOG_DEBUG);
	testing::internal::CaptureStdout();
	Log::setLogLevel(LOG_ERR);
	actor.drawConfig();
	string output = testing::internal::GetCapturedStdout();
	EXPECT_NE(output.find("Direction: Forward Bouncer"), string::npos);
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
