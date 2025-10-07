/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      ColorsTest.cpp
 * @since     Aug 21, 2025
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
#include "utilities/Colors.hpp"

using namespace LEDSpicer::Utilities;

TEST(TestColors, DrawColors) {

	// Empty: no output
	testing::internal::CaptureStdout();
	Colors c1("");
	c1.drawColors();
	std::string output = testing::internal::GetCapturedStdout();
	EXPECT_TRUE(output.empty());

	// Draw: check output
	Color::loadColors({{"Red", "FF0000"}, {"Green", "00FF00"}, {"Blue", "0000FF"}}, "hex");
	Colors c2("Red,Green,Blue");
	testing::internal::CaptureStdout();
	c2.drawColors();
	output = testing::internal::GetCapturedStdout();
	EXPECT_EQ(output,  "Red, Green, Blue");
}

// Main function for running tests
int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
