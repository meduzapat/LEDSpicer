/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DirectionActorTest.cpp
 * @since     Sep 3, 2025
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
#include "animations/DirectionActor.hpp"

using namespace LEDSpicer::Animations;
using namespace LEDSpicer::Devices;
using LEDSpicer::Utilities::Color;
using LEDSpicer::Utilities::Direction;

/// Global default color for tests.
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

	/// Sets stepping for testing.
	void setStepping(FrameActor::Stepping s) {
		stepping = s;
	}

	/// Returns the current runtime direction state (inherited Direction base).
	Direction getCurrentDirection() const {
		return static_cast<const Direction&>(*this);
	}

	/// Returns whether the actor is configured as a bouncer (immutable config).
	bool isBouncerConfig() const {
		return initDir.isBouncer();
	}

	/// Returns the current frame.
	uint16_t getCurrentFrame() const {
		return stepping.frame;
	}

protected:

	void calculateElements() override {
		affectAllElements(true);
	}
};

} // namespace

/// Shared stepping setup helper.
static FrameActor::Stepping makeNormalStepping() {
	FrameActor::Stepping s;
	s.frames = FrameActor::calculateFramesBySpeed(Speed::Speeds::Normal);
	return s;
}

/// Shared actor factory.
static TestDirectionActor makeActor(
	Group&             group,
	const string&      direction,
	bool               bouncer = false,
	uint8_t            startAt = 0
) {
	StringUMap params {
		{"type",      "Test"},
		{"group",     "TestGroup"},
		{"filter",    "Normal"},
		{"speed",     "Normal"},
		{"direction", direction},
	};
	if (bouncer) params["bouncer"] = "True";
	if (startAt) params["startAt"] = to_string(startAt);
	const vector<string> required {"type", "group", "filter", "speed", "direction"};
	return TestDirectionActor(params, &group, required);
}

TEST(DirectionActorTest, ConstructorForward) {
	Actor::setFPS(60);
	Group group("TestGroup", DEFAULT_COLOR);
	auto  actor = makeActor(group, "Forward");
	actor.setStepping(makeNormalStepping());
	EXPECT_TRUE(actor.isForward());
	EXPECT_FALSE(actor.isBackward());
	EXPECT_FALSE(actor.isBouncing());
	EXPECT_EQ(actor.getCurrentFrame(), 0);
	EXPECT_EQ(actor.getFullFrames(), 60);
}

TEST(DirectionActorTest, ConstructorBackward) {
	Actor::setFPS(60);
	Group group("TestGroup", DEFAULT_COLOR);
	auto  actor = makeActor(group, "Backward");
	actor.setStepping(makeNormalStepping());
	actor.restart();
	EXPECT_FALSE(actor.isForward());
	EXPECT_TRUE(actor.isBackward());
	EXPECT_FALSE(actor.isBouncing());
	EXPECT_EQ(actor.getCurrentFrame(), 59);
	EXPECT_EQ(actor.getFullFrames(), 60);
}

TEST(DirectionActorTest, ConstructorBouncer) {
	Actor::setFPS(60);
	Group group("TestGroup", DEFAULT_COLOR);
	auto  actor = makeActor(group, "Forward", true);
	actor.setStepping(makeNormalStepping());
	actor.restart();
	EXPECT_TRUE(actor.isForward());
	EXPECT_FALSE(actor.isBackward());
	EXPECT_TRUE(actor.isBouncerConfig());
	EXPECT_EQ(actor.getCurrentFrame(), 0);
	// Bouncer doubles the frame count.
	EXPECT_EQ(actor.getFullFrames(), 120);
}

TEST(DirectionActorTest, ConstructorInvalidParams) {
	Group group("TestGroup", DEFAULT_COLOR);
	StringUMap params {
		{"type",   "Test"},
		{"group",  "TestGroup"},
		{"filter", "Normal"},
		{"speed",  "Normal"},
		// Missing "direction"
	};
	const vector<string> required {"type", "group", "filter", "speed", "direction"};
	EXPECT_THROW(TestDirectionActor(params, &group, required), Error);
}

TEST(DirectionActorTest, StartAtForward) {
	Actor::setFPS(60);
	Group group("TestGroup", DEFAULT_COLOR);
	auto  actor = makeActor(group, "Forward", false, 50);
	actor.setStepping(makeNormalStepping());
	actor.restart();
	// startAt=50%: (60 * (50 - 1)) / 100 = 29
	EXPECT_EQ(actor.getCurrentFrame(), 29);
}

TEST(DirectionActorTest, StartAtBackward) {
	Actor::setFPS(60);
	Group group("TestGroup", DEFAULT_COLOR);
	auto  actor = makeActor(group, "Backward", false, 50);
	actor.setStepping(makeNormalStepping());
	actor.restart();
	// startAt=50% mirrored: (60 * (100 - 50)) / 100 = 30
	EXPECT_EQ(actor.getCurrentFrame(), 30);
}

TEST(DirectionActorTest, StartAtBouncer) {
	Actor::setFPS(60);
	Group group("TestGroup", DEFAULT_COLOR);
	auto  actor = makeActor(group, "Forward", true, 50);
	actor.setStepping(makeNormalStepping());
	actor.restart();
	// Bouncer starts forward, same as forward startAt.
	EXPECT_EQ(actor.getCurrentFrame(), 29);
	EXPECT_TRUE(actor.getCurrentDirection().isForward());
}

TEST(DirectionActorTest, StartOfCycle) {
	Actor::setFPS(60);
	Group group("TestGroup", DEFAULT_COLOR);

	// Forward.
	{
		auto actor = makeActor(group, "Forward");
		auto stepping = makeNormalStepping();
		stepping.steps = 2;
		actor.setStepping(stepping);
		actor.restart();
		EXPECT_TRUE(actor.isStartOfCycle());   // frame == 0, step == 0
		EXPECT_TRUE(actor.isFirstFrame());
		actor.draw();
		EXPECT_FALSE(actor.isStartOfCycle());  // frame == 0, step == 1
	}

	// Backward.
	{
		auto actor = makeActor(group, "Backward");
		auto stepping = makeNormalStepping();
		stepping.steps = 2;
		actor.setStepping(stepping);
		actor.restart();
		EXPECT_TRUE(actor.isStartOfCycle());   // frame == 59, step == 0
		EXPECT_TRUE(actor.isFirstFrame());
		actor.draw();
		EXPECT_FALSE(actor.isStartOfCycle());  // frame == 59, step == 1
	}

	// Bouncer — start of cycle at frame 0 going forward, and at frame 59 going backward.
	{
		auto actor = makeActor(group, "Forward", true);
		auto stepping = makeNormalStepping();
		stepping.steps = 2;
		actor.setStepping(stepping);
		actor.restart();
		EXPECT_TRUE(actor.isStartOfCycle());   // frame == 0, step == 0, forward
		EXPECT_TRUE(actor.isFirstFrame());
		EXPECT_TRUE(actor.getCurrentDirection().isForward());
		// Advance to bounce point.
		for (uint16_t i = 0; i < stepping.frames * stepping.steps; ++i) actor.draw();
		EXPECT_TRUE(actor.isStartOfCycle());   // frame == 59, step == 0, backward
		EXPECT_TRUE(actor.getCurrentDirection().isBackward());
	}
}

TEST(DirectionActorTest, IsLastFrameOverride) {
	Actor::setFPS(60);
	Group group("TestGroup", DEFAULT_COLOR);

	// Forward: last frame is FrameActor::isLastFrame().
	{
		auto actor = makeActor(group, "Forward");
		actor.setStepping(makeNormalStepping());
		actor.restart();
		EXPECT_FALSE(actor.isLastFrame());
		for (uint16_t i = 0; i < 59; ++i) actor.draw();
		EXPECT_TRUE(actor.isLastFrame());
	}

	// Backward: last frame is FrameActor::isFirstFrame() (frame == 0).
	{
		auto actor = makeActor(group, "Backward");
		actor.setStepping(makeNormalStepping());
		actor.restart();
		EXPECT_FALSE(actor.isLastFrame());
		for (uint16_t i = 0; i < 59; ++i) actor.draw();
		EXPECT_TRUE(actor.isLastFrame());
	}
}

TEST(DirectionActorTest, IsEndOfCycle) {
	Actor::setFPS(60);
	Group group("TestGroup", DEFAULT_COLOR);

	// Forward non-bouncer: end of cycle at last frame + last step.
	{
		auto actor   = makeActor(group, "Forward");
		auto stepping = makeNormalStepping();
		stepping.steps = 2;
		actor.setStepping(stepping);
		actor.restart();
		EXPECT_FALSE(actor.isEndOfCycle());
		const uint16_t totalDraws = (stepping.frames * stepping.steps) - 1;
		for (uint16_t i = 0; i < totalDraws; ++i) actor.draw();
		EXPECT_TRUE(actor.isEndOfCycle());
	}

	// Bouncer: end of cycle only when bouncing back to start.
	{
		auto actor   = makeActor(group, "Forward", true);
		auto stepping = makeNormalStepping();
		actor.setStepping(stepping);
		actor.restart();
		// At the midpoint (end of forward leg) not yet end of cycle.
		for (uint16_t i = 0; i < stepping.frames - 1; ++i) actor.draw();
		EXPECT_FALSE(actor.isEndOfCycle());
	}
}

TEST(DirectionActorTest, ForwardFrameAdvance) {
	Actor::setFPS(60);
	Group group("TestGroup", DEFAULT_COLOR);
	auto  actor    = makeActor(group, "Forward");
	auto  stepping = makeNormalStepping();
	actor.setStepping(stepping);
	actor.restart();
	EXPECT_EQ(actor.getCurrentFrame(), 0);
	actor.draw();
	EXPECT_EQ(actor.getCurrentFrame(), 1);
	for (uint16_t i = 0; i < 57; ++i) actor.draw();
	EXPECT_EQ(actor.getCurrentFrame(), 58);
	EXPECT_FALSE(actor.isLastFrame());
	actor.draw();
	EXPECT_TRUE(actor.isLastFrame());
	actor.draw();
	// Wraps around.
	EXPECT_EQ(actor.getCurrentFrame(), 0);
}

TEST(DirectionActorTest, BackwardFrameAdvance) {
	Actor::setFPS(60);
	Group group("TestGroup", DEFAULT_COLOR);
	auto  actor   = makeActor(group, "Backward");
	auto  stepping = makeNormalStepping();
	actor.setStepping(stepping);
	actor.restart();
	EXPECT_EQ(actor.getCurrentFrame(), 59);
	actor.draw();
	EXPECT_EQ(actor.getCurrentFrame(), 58);
	for (uint16_t i = 0; i < 57; ++i) actor.draw();
	EXPECT_EQ(actor.getCurrentFrame(), 1);
	EXPECT_FALSE(actor.isLastFrame());
	actor.draw();
	EXPECT_TRUE(actor.isLastFrame());
	actor.draw();
	// Wraps around.
	EXPECT_EQ(actor.getCurrentFrame(), 59);
}

TEST(DirectionActorTest, BouncerFrameAdvance) {
	Actor::setFPS(60);
	Group group("TestGroup", DEFAULT_COLOR);
	auto  actor   = makeActor(group, "Forward", true);
	auto  stepping = makeNormalStepping();
	actor.setStepping(stepping);
	actor.restart();
	EXPECT_TRUE(actor.getCurrentDirection().isForward());
	EXPECT_EQ(actor.getCurrentFrame(), 0);
	const uint16_t lastFrame = stepping.frames - 1;
	for (uint16_t i = 0; i < lastFrame; ++i) actor.draw();
	EXPECT_EQ(actor.getCurrentFrame(), lastFrame);
	EXPECT_TRUE(actor.isLastFrame());
	actor.draw();
	// Direction reverses, stays at last frame.
	EXPECT_FALSE(actor.getCurrentDirection().isForward());
	EXPECT_EQ(actor.getCurrentFrame(), lastFrame);
	actor.draw();
	EXPECT_EQ(actor.getCurrentFrame(), lastFrame - 1);
}

TEST(DirectionActorTest, CalculateNextOf) {
	Direction direction(Direction::Directions::Forward, true);
	Direction currentDirection(Direction::Directions::Forward);
	uint16_t  index;

	// Forward, non-boundary.
	index = DirectionActor::calculateNextOf(currentDirection, 5, direction, 9);
	EXPECT_EQ(index, 6);
	EXPECT_TRUE(currentDirection.isForward());
	EXPECT_FALSE(currentDirection.isBouncer());

	// Forward, at end, bouncer — reverses.
	index = DirectionActor::calculateNextOf(currentDirection, 9, direction, 9);
	EXPECT_EQ(index, 9);
	EXPECT_FALSE(currentDirection.isForward());
	EXPECT_TRUE(currentDirection.isBouncer());

	// Backward, non-boundary.
	index = DirectionActor::calculateNextOf(currentDirection, 5, direction, 9);
	EXPECT_EQ(index, 4);
	EXPECT_FALSE(currentDirection.isForward());

	// Backward, at start, bouncer — reverses.
	index = DirectionActor::calculateNextOf(currentDirection, 0, direction, 9);
	EXPECT_EQ(index, 0);
	EXPECT_TRUE(currentDirection.isForward());
}

TEST(DirectionActorTest, NextOf) {
	Direction direction(Direction::Directions::Forward, true);
	Direction currentDirection(Direction::Directions::Forward);
	uint16_t  index;

	// Forward, non-boundary.
	index = DirectionActor::nextOf(currentDirection, 5, direction, 9);
	EXPECT_EQ(index, 6);

	// Forward, at end, bouncer.
	index = DirectionActor::nextOf(currentDirection, 9, direction, 9);
	EXPECT_EQ(index, 9);

	// Backward, non-boundary.
	currentDirection = Direction::Directions::Backward;
	index = DirectionActor::nextOf(currentDirection, 5, direction, 9);
	EXPECT_EQ(index, 4);

	// Backward, at start, bouncer.
	index = DirectionActor::nextOf(currentDirection, 0, direction, 9);
	EXPECT_EQ(index, 0);
}

TEST(DirectionActorTest, DrawConfig) {
	Actor::setFPS(60);
	Group group("TestGroup", DEFAULT_COLOR);
	auto  actor = makeActor(group, "Forward", true);
	actor.restart();
	testing::internal::CaptureStdout();
	actor.drawConfig();
	string output = testing::internal::GetCapturedStdout();
	EXPECT_NE(output.find("Direction: Forward Bouncer"), string::npos);
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
