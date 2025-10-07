/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      LogTest.cpp
 * @since     Aug 22, 2025
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
#include "utilities/Log.hpp"

using namespace LEDSpicer::Utilities;

class LogTest : public ::testing::Test {

protected:

	void SetUp() override {
		// Ensure logging to stdout for tests.
		Log::logToStdTerm(true);
		// Set to max level to log everything by default.
		Log::setLogLevel(LOG_DEBUG);
	}

	void TearDown() override {
		// Reset to default after each test.
		Log::setLogLevel(LOG_NOTICE);
	}
};

TEST_F(LogTest, LevelConversions) {
	EXPECT_EQ(Log::str2level("Error"),   LOG_ERR);
	EXPECT_EQ(Log::str2level("Warning"), LOG_WARNING);
	EXPECT_EQ(Log::str2level("Notice"),  LOG_NOTICE);
	EXPECT_EQ(Log::str2level("Info"),    LOG_INFO);
	EXPECT_EQ(Log::str2level("Debug"),   LOG_DEBUG);

	// Invalid defaults to INFO.
	testing::internal::CaptureStderr();
	EXPECT_EQ(Log::str2level("Invalid"), LOG_INFO);
	string output = testing::internal::GetCapturedStderr();
	EXPECT_TRUE(output.find("Invalid log level Invalid assuming Info") != string::npos);

	EXPECT_EQ(Log::level2str(LOG_ERR),     "Error");
	EXPECT_EQ(Log::level2str(LOG_WARNING), "Warning");
	EXPECT_EQ(Log::level2str(LOG_NOTICE),  "Notice");
	EXPECT_EQ(Log::level2str(LOG_INFO),    "Info");
	EXPECT_EQ(Log::level2str(LOG_DEBUG),   "Debug");
	EXPECT_EQ(Log::level2str(100),         "Unknown");
}

TEST_F(LogTest, SetAndGetLogLevel) {
	Log::setLogLevel(LOG_WARNING);
	EXPECT_EQ(Log::getLogLevel(), LOG_WARNING);

	Log::setLogLevel(LOG_DEBUG);
	EXPECT_EQ(Log::getLogLevel(), LOG_DEBUG);
}

TEST_F(LogTest, IsLogging) {
	Log::setLogLevel(LOG_NOTICE);
	EXPECT_TRUE(Log::isLogging(LOG_ERR));
	EXPECT_TRUE(Log::isLogging(LOG_WARNING));
	EXPECT_TRUE(Log::isLogging(LOG_NOTICE));
	EXPECT_FALSE(Log::isLogging(LOG_INFO));
	EXPECT_FALSE(Log::isLogging(LOG_DEBUG));
}

TEST_F(LogTest, LogToStdOutLevels) {
	// Test debug (cout).
	testing::internal::CaptureStdout();
	Log::debug("Test debug message");
	string output = testing::internal::GetCapturedStdout();
	EXPECT_EQ(output, "Test debug message\n");

	// Test info (cout).
	testing::internal::CaptureStdout();
	Log::info("Test info message");
	output = testing::internal::GetCapturedStdout();
	EXPECT_EQ(output, "Test info message\n");

	// Test notice (cout).
	testing::internal::CaptureStdout();
	Log::notice("Test notice message");
	output = testing::internal::GetCapturedStdout();
	EXPECT_EQ(output, "Test notice message\n");

	// Test warning (cout).
	testing::internal::CaptureStdout();
	Log::warning("Test warning message");
	output = testing::internal::GetCapturedStdout();
	EXPECT_EQ(output, "Test warning message\n");

	// Test error (cerr).
	testing::internal::CaptureStderr();
	Log::error("Test error message");
	output = testing::internal::GetCapturedStderr();
	EXPECT_EQ(output, "Test error message\n");
}

TEST_F(LogTest, LogMacrosAndFiltering) {
	Log::setLogLevel(LOG_INFO);

	// Should log (INFO <= INFO).
	testing::internal::CaptureStdout();
	LogInfo("Test macro info");
	string output = testing::internal::GetCapturedStdout();
	EXPECT_EQ(output, "Test macro info\n");

	// Should not log (DEBUG > INFO).
	testing::internal::CaptureStdout();
	LogDebug("Should not log debug");
	output = testing::internal::GetCapturedStdout();
	EXPECT_EQ(output, "");

	// Test error macro (always logs if level allows).
	testing::internal::CaptureStderr();
	LogError("Test macro error");
	output = testing::internal::GetCapturedStderr();
	EXPECT_EQ(output, "Test macro error\n");
}

TEST_F(LogTest, InitializeAndTerminate) {
	// Initialize to stdout (already set in SetUp).
	testing::internal::CaptureStdout();
	Log::initialize(true);
	Log::info("Test after init stdout");
	string output = testing::internal::GetCapturedStdout();
	EXPECT_EQ(output, "Test after init stdout\n");

	// Switch to stdout again (no-op).
	Log::logToStdTerm(true);
	testing::internal::CaptureStdout();
	Log::info("Test after redundant stdout");
	output = testing::internal::GetCapturedStdout();
	EXPECT_EQ(output, "Test after redundant stdout\n");

	// Terminate (no-op for stdout).
	Log::terminate();
	testing::internal::CaptureStdout();
	Log::info("Test after terminate stdout");
	output = testing::internal::GetCapturedStdout();
	EXPECT_EQ(output, "Test after terminate stdout\n");
}

// Note: Syslog tests skipped to avoid system log pollution; focus on stdout.

// Main function for running tests
int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
