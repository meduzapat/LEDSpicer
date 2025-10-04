/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      SpeedTest.cpp
 * @since     Aug 23, 2025
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
#include "utilities/Speed.hpp"

using namespace LEDSpicer::Utilities;

TEST(SpeedTest, ConstructorValid) {
	Speed speedObj("Fast");
	testing::internal::CaptureStdout();
	speedObj.drawConfig();
	string output = testing::internal::GetCapturedStdout();
	EXPECT_EQ(output, "Speed: Fast\n");

	Speed speedObj2(""); // Empty defaults to Normal.
	testing::internal::CaptureStdout();
	speedObj2.drawConfig();
	output = testing::internal::GetCapturedStdout();
	EXPECT_EQ(output, "Speed: Normal\n");
}

TEST(SpeedTest, ConstructorInvalid) {
	EXPECT_THROW(Speed("InvalidSpeed"), Error);
}

TEST(SpeedTest, Str2SpeedValid) {
	EXPECT_EQ(Speed::str2speed("VeryFast"), Speed::Speeds::VeryFast);
	EXPECT_EQ(Speed::str2speed("Fast"),     Speed::Speeds::Fast);
	EXPECT_EQ(Speed::str2speed("Normal"),   Speed::Speeds::Normal);
	EXPECT_EQ(Speed::str2speed("Slow"),     Speed::Speeds::Slow);
	EXPECT_EQ(Speed::str2speed("VerySlow"), Speed::Speeds::VerySlow);
}

TEST(SpeedTest, Str2SpeedInvalid) {
	EXPECT_THROW(Speed::str2speed("Invalid"), Error);
	 // Empty throws, unlike constructor.
	EXPECT_THROW(Speed::str2speed(""), Error);
}

TEST(SpeedTest, Speed2StrValid) {
	EXPECT_EQ(Speed::speed2str(Speed::Speeds::VeryFast), "Very Fast");
	EXPECT_EQ(Speed::speed2str(Speed::Speeds::Fast),     "Fast");
	EXPECT_EQ(Speed::speed2str(Speed::Speeds::Normal),   "Normal");
	EXPECT_EQ(Speed::speed2str(Speed::Speeds::Slow),     "Slow");
	EXPECT_EQ(Speed::speed2str(Speed::Speeds::VerySlow), "Very Slow");
}

TEST(SpeedTest, Speed2StrInvalid) {
	// Enum out of range; returns empty string as per code.
	EXPECT_EQ(Speed::speed2str(static_cast<Speed::Speeds>(5)), "");
}

TEST(SpeedTest, DrawConfig) {
	Speed speedObj("VerySlow");
	testing::internal::CaptureStdout();
	speedObj.drawConfig();
	string output = testing::internal::GetCapturedStdout();
	EXPECT_EQ(output, "Speed: Very Slow\n");

	Speed speedObj2("Normal");
	testing::internal::CaptureStdout();
	speedObj2.drawConfig();
	output = testing::internal::GetCapturedStdout();
	EXPECT_EQ(output, "Speed: Normal\n");
}

// Main function for running tests
int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
