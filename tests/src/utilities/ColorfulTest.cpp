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
#include "utilities/Colorful.hpp"

using namespace LEDSpicer::Utilities;

class TestColorful : public Color, public ::testing::Test {

public:

	static void clearColors() {
		colors.clear();
		names.clear();
	}

protected:
	void SetUp() override {
		clearColors();
	}
};

TEST_F(TestColorful, Cycle) {

	Color::loadColors({{"Red", "FF0000"}, {"Green", "00FF00"}, {"Blue", "0000FF"}}, "hex");

	Colorful cf("Red,Green,Blue");

	// Initial color
	EXPECT_EQ(cf.getCurrentColor()->getR(), 255);
	EXPECT_EQ(cf.getCurrentColor()->getG(), 0);
	EXPECT_EQ(cf.getCurrentColor()->getB(), 0);

	// Advance to Green
	cf.advanceColor();
	EXPECT_EQ(cf.getCurrentColor()->getR(), 0);
	EXPECT_EQ(cf.getCurrentColor()->getG(), 255);
	EXPECT_EQ(cf.getCurrentColor()->getB(), 0);

	// Advance to Blue
	cf.advanceColor();
	EXPECT_EQ(cf.getCurrentColor()->getR(), 0);
	EXPECT_EQ(cf.getCurrentColor()->getG(), 0);
	EXPECT_EQ(cf.getCurrentColor()->getB(), 255);

	// Advance wraps to Red
	cf.advanceColor();
	EXPECT_EQ(cf.getCurrentColor()->getR(), 255);
	EXPECT_EQ(cf.getCurrentColor()->getG(), 0);
	EXPECT_EQ(cf.getCurrentColor()->getB(), 0);

	// Reset to Red
	cf.advanceColor(); // To Green
	cf.reset();
	EXPECT_EQ(cf.getCurrentColor()->getR(), 255);
	EXPECT_EQ(cf.getCurrentColor()->getG(), 0);
	EXPECT_EQ(cf.getCurrentColor()->getB(), 0);
}

TEST_F(TestColorful, EdgeCases) {
	Color::loadColors({{"Red", "FF0000"}}, "hex");

	// Single color
	Colorful cf1("Red");
	EXPECT_EQ(cf1.getCurrentColor()->getR(), 255);
	cf1.advanceColor();
	EXPECT_EQ(cf1.getCurrentColor()->getR(), 255); // Wraps to itself
	cf1.reset();
	EXPECT_EQ(cf1.getCurrentColor()->getR(), 255);

	// Empty
	Colorful cf2("");
	EXPECT_EQ(cf2.getCurrentColor()->getName(), "Red");
	cf2.advanceColor();
	EXPECT_EQ(cf2.getCurrentColor()->getName(), "Red");
	cf2.advanceColor();
	EXPECT_EQ(cf2.getCurrentColor()->getName(), "Red");
	cf2.reset();
	EXPECT_EQ(cf2.getCurrentColor()->getName(), "Red");
}

// Main function for running tests
int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
