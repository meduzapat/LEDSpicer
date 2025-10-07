/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      TimeTest.cpp
 * @since     Aug 23, 2025
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
#include "utilities/Time.hpp"

using namespace LEDSpicer::Utilities;

TEST(TimeTest, ConstructorZeroSeconds) {
	// Immediate expiration.
	EXPECT_TRUE(Time(0).isTime());
	Time time;
	EXPECT_TRUE(time.isTime());
}

TEST(TimeTest, IsTimeExpiration) {
	// 2 seconds.
	Time timeObj(2);
	EXPECT_FALSE(timeObj.isTime()); // Not yet.
	sleep_for(std::chrono::milliseconds(500)); // Wait 0.5s.
	EXPECT_FALSE(timeObj.isTime()); // Still not.
	sleep_for(std::chrono::milliseconds(1500)); // Wait 1.5s.
	// Check multiple calls remain true.
	EXPECT_TRUE(timeObj.isTime());
	EXPECT_TRUE(timeObj.isTime());
}

TEST(TimeTest, MultipleTimersIndependent) {
	Time timeShort(1);
	Time timeLong(3);
	sleep_for(std::chrono::milliseconds(1500)); // Wait 1.5s.
	EXPECT_TRUE(timeShort.isTime());
	EXPECT_FALSE(timeLong.isTime());
	sleep_for(std::chrono::milliseconds(2000)); // Wait another 2s.
	EXPECT_TRUE(timeLong.isTime());
}

TEST(TimeTest, ResetTimer) {
	Time timeObj(1);
	sleep_for(std::chrono::milliseconds(500));
	timeObj.reset(1000);
	// Reset to 1s.
	EXPECT_FALSE(timeObj.isTime());
	sleep_for(std::chrono::milliseconds(1100));
	EXPECT_TRUE(timeObj.isTime());
}

// Main function for running tests
int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
