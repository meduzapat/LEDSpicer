/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DrySerialTest.cpp
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
#include "utilities/Serial.hpp"

using namespace LEDSpicer::Utilities;

class TestSerial : public Serial {

public:

	using Serial::Serial;

	using Serial::detectPort;

	using Serial::connect;

	using Serial::disconnect;

	using Serial::transferToConnection;

	using Serial::transferFromConnection;

	static string findPortByUsbId(const string& id) {
		return Serial::findPortByUsbId(id);
	}
};

class DrySerialTest : public ::testing::Test {

protected:

	void SetUp() override {
		// Set debug level to capture logs.
		Log::setLogLevel(LOG_ERR);
		// Log to stdout for capture.
		Log::logToStdTerm(true);
	}
};

TEST_F(DrySerialTest, ConnectDisconnect) {
	TestSerial s("testport");

	// Capture connect logs.
	Log::setLogLevel(LOG_DEBUG);
	testing::internal::CaptureStdout();
	s.connect();
	string output = testing::internal::GetCapturedStdout();
	EXPECT_TRUE(output.find("Opening connection to testport") != string::npos);
	EXPECT_TRUE(output.find("No Serial connection - DRY RUN") != string::npos);

	// Capture disconnect logs.
	testing::internal::CaptureStdout();
	s.disconnect();
	output = testing::internal::GetCapturedStdout();
	EXPECT_TRUE(output.find("Closing connection to testport") != string::npos);
	EXPECT_TRUE(output.find("No disconnect - DRY RUN") != string::npos);

	// No active connection.
	testing::internal::CaptureStdout();
	s.disconnect();
	output = testing::internal::GetCapturedStdout();
	EXPECT_TRUE(output.find("No active connection to close") != string::npos);
	Log::setLogLevel(LOG_ERR);
}

TEST_F(DrySerialTest, TransferConnection) {
	TestSerial s("testport");
	s.connect(); // Dry run.

	vector<uint8_t> data = {1, 2, 3};
	Log::setLogLevel(LOG_DEBUG);
	testing::internal::CaptureStdout();
	s.transferToConnection(data);
	string output = testing::internal::GetCapturedStdout();
	EXPECT_TRUE(output.find("Data sent:") != string::npos);
	EXPECT_TRUE(output.find("01 02 03") != string::npos);
	Log::setLogLevel(LOG_ERR);

	vector<uint8_t> expected = {'o', 'k', '\n'};
	vector<uint8_t> result = s.transferFromConnection(3);
	EXPECT_EQ(result, expected);
}

// Main function for running tests
int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
