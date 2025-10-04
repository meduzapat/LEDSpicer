/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      MonochromaticTest.cpp
 * @since     Aug 22, 2025
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
#include "utilities/Monochromatic.hpp"

using namespace LEDSpicer::Utilities;

class MonochromaticTest : public ::testing::Test {

protected:
	StringUMap emptyOptions;
	string     deviceName = "TestDevice";
};

TEST_F(MonochromaticTest, CustomChangePoint) {
	StringUMap options = {{"changePoint", "128"}};
	Monochromatic mono(options, deviceName);
	EXPECT_EQ(mono.getChangePoint(), 128);
}

TEST_F(MonochromaticTest, InvalidChangePoint) {
	StringUMap options = {{"changePoint", "abc"}};
	EXPECT_THROW(Monochromatic(options, deviceName), Error);
}

TEST_F(MonochromaticTest, OutOfRangeChangePoint) {
	// If the value is less than 10 will be rounded up.
	StringUMap options1 = {{"changePoint", "-1"}};
	Monochromatic mono1(options1, deviceName);
	EXPECT_EQ(mono1.getChangePoint(), 10);
	// If the value is less more than 245 will be rounded down.
	StringUMap options2 = {{"changePoint", "256"}};
	Monochromatic mono2(options2, deviceName);
	EXPECT_EQ(mono2.getChangePoint(), 245);
}

TEST_F(MonochromaticTest, MissingChangePoint) {
	StringUMap options = {{"otherKey", "value"}};
	Monochromatic mono(options, deviceName);
	EXPECT_EQ(mono.getChangePoint(), 64);
}

// Main function for running tests
int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
