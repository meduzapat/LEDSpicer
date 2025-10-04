/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DirectionTest.cpp
 * @since     Aug 21, 2025
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
#include <sstream>
#include "utilities/Direction.hpp"

using namespace LEDSpicer::Utilities;

TEST(DirectionTest, DefaultConstructor) {
	Direction d;
	EXPECT_EQ(d.getDirection(), Direction::Directions::Forward);
	EXPECT_FALSE(d.isBouncer());
}

TEST(DirectionTest, ConstructorValid) {

	Direction d1("Forward");
	EXPECT_EQ(d1.getDirection(), Direction::Directions::Forward);
	EXPECT_FALSE(d1.isBouncer());

	Direction d2("Backward");
	EXPECT_EQ(d2.getDirection(), Direction::Directions::Backward);
	EXPECT_FALSE(d1.isBouncer());

	Direction d3("Stop");
	EXPECT_EQ(d3.getDirection(), Direction::Directions::Stop);
	EXPECT_FALSE(d3.isBouncer());

	Direction d4(Direction::Directions::Forward, true);
	EXPECT_EQ(d4.getDirection(), Direction::Directions::Forward);
	EXPECT_TRUE(d4.isBouncer());

	Direction d5(Direction::Directions::Backward, true);
	EXPECT_EQ(d5.getDirection(), Direction::Directions::Backward);
	EXPECT_TRUE(d5.isBouncer());

	Direction d6(Direction::Directions::Stop, true);
	EXPECT_EQ(d6.getDirection(), Direction::Directions::Stop);
	EXPECT_FALSE(d6.isBouncer());
}

TEST(DirectionTest, ConstructorInvalid) {
	EXPECT_THROW(Direction(""), Error);
	EXPECT_THROW(Direction("Invalid"), Error);
	EXPECT_THROW(Direction("ForwardExtra"), Error);
	EXPECT_THROW(Direction("StopBouncingExtra"), Error);
}

TEST(DirectionTest, Direction2Str) {
	EXPECT_EQ(Direction::direction2str(Direction::Directions::Forward),  "Forward");
	EXPECT_EQ(Direction::direction2str(Direction::Directions::Backward), "Backward");
	EXPECT_EQ(Direction::direction2str(Direction::Directions::Stop),     "Stop");
	EXPECT_EQ(Direction::direction2str(static_cast<Direction::Directions>(100)), "");
}

TEST(DirectionTest, Str2Direction) {
	EXPECT_EQ(Direction::str2direction("Forward"), Direction::Directions::Forward);
	EXPECT_EQ(Direction::str2direction("Backward"), Direction::Directions::Backward);
	EXPECT_EQ(Direction::str2direction("Stop"), Direction::Directions::Stop);
	EXPECT_THROW(Direction::str2direction("Invalid"), Error);
	EXPECT_THROW(Direction::str2direction(""), Error);
	EXPECT_THROW(Direction::str2direction("forward"), Error); // Case sensitive
}

TEST(DirectionTest, GetOppositeDirectionAndDirections) {
	Direction d1(Direction::Directions::Forward);
	Direction d2(Direction::Directions::Backward);
	Direction d3(Direction::Directions::Stop);
	EXPECT_EQ(d1.getOppositeDirection(), Direction::Directions::Backward);
	EXPECT_EQ(d2.getOppositeDirection(), Direction::Directions::Forward);
	EXPECT_EQ(d3.getOppositeDirection(), Direction::Directions::Stop);

	EXPECT_TRUE(d1.isForward());
	EXPECT_FALSE(d1.isBackward());
	EXPECT_FALSE(d1.isStopped());

	EXPECT_TRUE(d2.isBackward());
	EXPECT_FALSE(d2.isForward());
	EXPECT_FALSE(d2.isStopped());

	EXPECT_TRUE(d3.isStopped());
	EXPECT_FALSE(d3.isForward());
	EXPECT_FALSE(d3.isBackward());
}

TEST(DirectionTest, Setters) {
	Direction d;
	EXPECT_EQ(d.getDirection(), Direction::Directions::Forward);
	EXPECT_FALSE(d.isBouncer());

	d.setDirection(Direction::Directions::Backward);
	EXPECT_EQ(d.getDirection(), Direction::Directions::Backward);
	EXPECT_FALSE(d.isBouncer());

	d.setBouncer(true);
	EXPECT_EQ(d.getDirection(), Direction::Directions::Backward);
	EXPECT_TRUE(d.isBouncer());

	d.setDirection(Direction::Directions::Stop);
	EXPECT_EQ(d.getDirection(), Direction::Directions::Stop);
	EXPECT_TRUE(d.isBouncer()); // Unlike constructor, setter does not reset bounce

	d.setBouncer(false);
	EXPECT_EQ(d.getDirection(), Direction::Directions::Stop);
	EXPECT_FALSE(d.isBouncer());

	d.setDirection(Direction::Directions::Forward);
	EXPECT_EQ(d.getDirection(), Direction::Directions::Forward);
	EXPECT_FALSE(d.isBouncer());
}

TEST(DirectionTest, Reverse) {
	Direction d1("Forward");
	EXPECT_EQ(d1.getDirection(), Direction::Directions::Forward);
	EXPECT_FALSE(d1.isBouncer());

	d1.reverse();
	EXPECT_EQ(d1.getDirection(), Direction::Directions::Backward);
	EXPECT_TRUE(d1.isBouncer());

	d1.reverse();
	EXPECT_EQ(d1.getDirection(), Direction::Directions::Forward);
	EXPECT_FALSE(d1.isBouncer());

	Direction d2("Backward", true);
	EXPECT_EQ(d2.getDirection(), Direction::Directions::Backward);
	EXPECT_TRUE(d2.isBouncer());

	d2.reverse();
	EXPECT_EQ(d2.getDirection(), Direction::Directions::Forward);
	EXPECT_FALSE(d2.isBouncer());

	d2.reverse();
	EXPECT_EQ(d2.getDirection(), Direction::Directions::Backward);
	EXPECT_TRUE(d2.isBouncer());

	Direction d3("Stop");
	EXPECT_EQ(d3.getDirection(), Direction::Directions::Stop);
	EXPECT_FALSE(d3.isBouncer());

	d3.reverse();
	EXPECT_EQ(d3.getDirection(), Direction::Directions::Stop);
	EXPECT_TRUE(d3.isBouncer());

	d3.reverse();
	EXPECT_EQ(d3.getDirection(), Direction::Directions::Stop);
	EXPECT_FALSE(d3.isBouncer());
}

TEST(DirectionTest, DrawConfig) {
	Direction d1("Forward");
	testing::internal::CaptureStdout();
	d1.drawConfig();
	string output1 = testing::internal::GetCapturedStdout();
	EXPECT_EQ(output1, "Direction: Forward\n");

	Direction d2("Backward", true);
	testing::internal::CaptureStdout();
	d2.drawConfig();
	string output2 = testing::internal::GetCapturedStdout();
	EXPECT_EQ(output2, "Direction: Backward Bouncer\n");

	testing::internal::CaptureStdout();
	Direction d3("Stop");
	d3.drawConfig();
	string output3 = testing::internal::GetCapturedStdout();
	EXPECT_EQ(output3, "Direction: Stop\n");
}

// Main function for running tests
int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
