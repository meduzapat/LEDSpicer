/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      ErrorTest.cpp
 * @since     Aug 24, 2025
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
#include "utilities/Error.hpp"

using namespace LEDSpicer::Utilities;

TEST(ErrorTest, ConstructorAndGetMessage) {
	Error e("Test error message");
	EXPECT_EQ(e.getMessage(), "Test error message");
}

TEST(ErrorTest, Streaming) {
	Error e;
	e << "Invalid value: " << 42 << " or float " << 3.5;
	EXPECT_EQ(e.getMessage(), "Invalid value: 42 or float 3.5");
}

TEST(ErrorTest, EmptyMessage) {
	EXPECT_EQ(Error().getMessage(), "");
}

TEST(ErrorTest, AsException) {
	try {
		throw Error("Thrown error: ") << 123 << "abc" << 1.3;
	} catch (const Error& ex) {
		EXPECT_EQ(ex.getMessage(), "Thrown error: 123abc1.3");
	}
}

TEST(ErrorTest, CopyConstructor) {
	Error e1("Test");
	Error e2(e1);
	EXPECT_EQ(e2.getMessage(), "Test");
}

// Main function for running tests
int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
