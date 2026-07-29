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
		Log::setCategories("All");
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

TEST_F(LogTest, CategoryConversions) {
	EXPECT_EQ(Log::str2category("Core"),     Log::Categories::Core);
	EXPECT_EQ(Log::str2category("Actors"),   Log::Categories::Actors);
	EXPECT_EQ(Log::str2category("Inputs"),   Log::Categories::Inputs);
	EXPECT_EQ(Log::str2category("Devices"),  Log::Categories::Devices);
	EXPECT_EQ(Log::str2category("Messages"), Log::Categories::Messages);
	EXPECT_EQ(Log::str2category("Profiles"), Log::Categories::Profiles);
	EXPECT_EQ(Log::str2category("All"),      Log::Categories::All);

	// Invalid is reported and ignored.
	testing::internal::CaptureStderr();
	EXPECT_EQ(Log::str2category("Nope"), Log::Categories::None);
	string output = testing::internal::GetCapturedStderr();
	EXPECT_TRUE(output.find("Invalid trace category Nope ignored") != string::npos);
}

TEST_F(LogTest, SetAndReportCategories) {
	// Everything is active by default.
	EXPECT_EQ(Log::categories2str(), "All");

	// Spaces around the names are tolerated, order follows the enum not the list.
	Log::setCategories("Inputs, Actors");
	EXPECT_EQ(Log::categories2str(), "Actors,Inputs");
	EXPECT_TRUE(Log::tracing(Log::Categories::Actors));
	EXPECT_TRUE(Log::tracing(Log::Categories::Inputs));
	EXPECT_FALSE(Log::tracing(Log::Categories::Devices));

	// An unknown name contributes nothing but does not discard the valid ones.
	testing::internal::CaptureStderr();
	Log::setCategories("Devices,Nope");
	testing::internal::GetCapturedStderr();
	EXPECT_EQ(Log::categories2str(), "Devices");

	// Reporting the active set does not depend on the log level.
	Log::setLogLevel(LOG_ERR);
	EXPECT_EQ(Log::categories2str(), "Devices");
	EXPECT_FALSE(Log::tracing(Log::Categories::Devices));
}

TEST_F(LogTest, TracingNeedsBothLevelAndCategory) {
	Log::setCategories("Actors");

	EXPECT_TRUE(Log::tracing(Log::Categories::Actors));
	EXPECT_FALSE(Log::tracing(Log::Categories::Inputs));

	// Below Debug nothing is traced, whatever the categories say.
	Log::setLogLevel(LOG_INFO);
	EXPECT_FALSE(Log::tracing(Log::Categories::Actors));
}

TEST_F(LogTest, TraceEmitsOnceWhenLeavingScope) {
	testing::internal::CaptureStdout();
	{
		Log::Trace trace(Log::Categories::Actors);
		trace << "one" << " " << 2 << " ";
		trace << "three";
		// Nothing is emitted until the instance dies.
		EXPECT_EQ(testing::internal::GetCapturedStdout(), "");
		testing::internal::CaptureStdout();
	}
	EXPECT_EQ(testing::internal::GetCapturedStdout(), "one 2 three\n");
}

TEST_F(LogTest, TraceEmitsNothingWhenTheCategoryIsOff) {
	Log::setCategories("Inputs");
	testing::internal::CaptureStdout();
	{
		Log::Trace trace(Log::Categories::Actors);
		trace << "should not appear";
	}
	EXPECT_EQ(testing::internal::GetCapturedStdout(), "");
}

TEST_F(LogTest, TraceEmitsNothingWhenEmpty) {
	testing::internal::CaptureStdout();
	{
		Log::Trace trace(Log::Categories::Actors);
	}
	EXPECT_EQ(testing::internal::GetCapturedStdout(), "");
}

TEST_F(LogTest, TraceKeepsStreamStateBetweenInsertions) {
	// A manipulator applies to the value inserted by the next call, which only
	// works because the instance keeps one stream for the whole line.
	testing::internal::CaptureStdout();
	{
		Log::Trace trace(Log::Categories::Actors);
		trace << std::setw(4) << std::setfill('0') << 7;
	}
	EXPECT_EQ(testing::internal::GetCapturedStdout(), "0007\n");
}

TEST_F(LogTest, TraceSurvivesAnEarlyReturn) {
	// The reason the line is emitted on destruction: a function that returns in
	// the middle of building a line still produces a complete line.
	auto builds = [](bool leaveEarly) {
		Log::Trace trace(Log::Categories::Actors);
		trace << "start ";
		if (leaveEarly)
			return;
		trace << "end";
	};

	testing::internal::CaptureStdout();
	builds(true);
	EXPECT_EQ(testing::internal::GetCapturedStdout(), "start \n");

	testing::internal::CaptureStdout();
	builds(false);
	EXPECT_EQ(testing::internal::GetCapturedStdout(), "start end\n");
}

TEST_F(LogTest, TraceFlushStartsANewLine) {
	testing::internal::CaptureStdout();
	{
		Log::Trace trace(Log::Categories::Actors);
		trace << "first";
		trace.flush();
		trace << "second";
	}
	EXPECT_EQ(testing::internal::GetCapturedStdout(), "first\nsecond\n");
}

// Note: Syslog tests skipped to avoid system log pollution; focus on stdout.

// Main function for running tests
int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
