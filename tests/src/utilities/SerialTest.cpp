/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      SerialTest.cpp
 * @since     Aug 25, 2025
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

static constexpr char DUMMY_PORT[] = "/dev/null";

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

	int getFakeFD() const { return fakeFD; }
};

class SerialTest : public ::testing::Test {

protected:

	void SetUp() override {
		// Log to stdout for capture.
		Log::setLogLevel(LOG_ERR);
		Log::logToStdTerm(true);
	}
};

TEST_F(SerialTest, DetectPortThrows) {
	TestSerial s("");
	EXPECT_THROW(s.detectPort(), Error);
}

TEST_F(SerialTest, TestGetPort) {
	TestSerial s(DUMMY_PORT);
	EXPECT_EQ(s.getPort(), DUMMY_PORT);
}

TEST_F(SerialTest, ConnectThrows) {
	TestSerial s(DUMMY_PORT);
	Log::setLogLevel(LOG_DEBUG);
	// Connect should fail on dummy port (not a tty).
	testing::internal::CaptureStdout();
	EXPECT_THROW(s.connect(), Error);
	string output = testing::internal::GetCapturedStdout();
	EXPECT_TRUE(output.find("Opening connection to " + string(DUMMY_PORT)) != string::npos);
	Log::setLogLevel(LOG_ERR);

	try {
		s.connect();
		FAIL() << "Expected Error";
	}
	catch (Error& e) {
		string msg = e.getMessage();
		EXPECT_TRUE(msg.find("Error getting serial port settings") != string::npos ||
		            msg.find("Not a tty") != string::npos); // ENOTTY message.
	}
	s.disconnect();
}

TEST_F(SerialTest, ThrowsWithoutConnect) {
	TestSerial s(DUMMY_PORT);
	vector<uint8_t> data {1, 2, 3};
	EXPECT_THROW(s.transferToConnection(data), Error);
	EXPECT_THROW(s.transferFromConnection(3), Error);

	// Disconnect without connect.
	Log::setLogLevel(LOG_DEBUG);
	testing::internal::CaptureStdout();
	s.disconnect();
	string output = testing::internal::GetCapturedStdout();
	EXPECT_TRUE(output.find("Closing connection to " + string(DUMMY_PORT)) != string::npos);
	EXPECT_TRUE(output.find("No active connection to close") != string::npos);
}

TEST_F(SerialTest, FindPortByUsbIdNotFound) {
	// Assuming no match, returns empty.
	EXPECT_EQ(TestSerial::findPortByUsbId("nonexistent:id"), "");
}

TEST_F(SerialTest, SendAndReceive) {
	TestSerial serial("veryunlikely-to-exist");

	// DTR ioctl warning is ignored
	serial.connect();

	// --- Test writing: Serial -> master ---
	std::vector<uint8_t> out = {'H','i'};
	serial.transferToConnection(out);

	char buffer[16] = {0};
	ssize_t n = ::read(serial.getFakeFD(), buffer, sizeof(buffer));
	EXPECT_GT(n, 0);
	EXPECT_EQ(buffer[0], 'H');
	EXPECT_EQ(buffer[1], 'i');
	EXPECT_EQ(buffer[2], '\0');

	// --- Test reading: master -> Serial ---
	const char* reply = "ok\0";
	EXPECT_EQ(::write(serial.getFakeFD(), reply, 3), 3);

	std::vector<uint8_t> in = serial.transferFromConnection(16);
	EXPECT_EQ(in, (std::vector<uint8_t>{'o','k','\0'}));

	serial.disconnect();
}

// Main function for running tests
int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
