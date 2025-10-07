/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      MessagesTest.cpp
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
#include "utilities/Messages.hpp"
#include "utilities/Message.hpp"
#include "utilities/Log.hpp"

using namespace LEDSpicer::Utilities;

class MessagesTest : public ::testing::Test {

protected:

	void SetUp() override {
		// Use a high port number assuming it's free for testing.
		testPort = "15555";
		// Set log to stdout/stderr for test visibility.
		Log::logToStdTerm(true);
	}

	string testPort;
};

TEST_F(MessagesTest, ReadValidMessage) {

	Messages server(testPort);
	Socks client(LOCALHOST, testPort, false);

	// Create valid message using Message class.
	Message msg;
	msg.setType(Message::Types::LoadProfile);
	msg.setFlags(FLAG_FORCE_RELOAD | FLAG_NO_INPUTS);
	msg.addData("param1");
	msg.addData("param2");
	string packed = msg.toString();

	client.send(packed);

	// Small delay to allow UDP delivery.
	sleep_for(std::chrono::milliseconds(100));

	EXPECT_TRUE(server.read());

	Message received = server.getMessage();

	EXPECT_EQ(received.getType(), Message::Types::LoadProfile);
	EXPECT_EQ(received.getFlags(), FLAG_FORCE_RELOAD | FLAG_NO_INPUTS);

	auto data = received.getData();
	EXPECT_EQ(data.size(), 2);
	EXPECT_EQ(data[0], "param1");
	EXPECT_EQ(data[1], "param2");

	// No more messages.
	EXPECT_FALSE(server.read());
}

TEST_F(MessagesTest, ReadInvalidType) {
	Log::setLogLevel(LOG_DEBUG);
	Messages server(testPort);
	Socks client(LOCALHOST, testPort, false);

	// Craft invalid type (assuming Invalid is 0).
	string packed(
		"param" + string(1, RECORD_SEPARATOR) +
		"0"     + string(1, RECORD_SEPARATOR) +
		"0"     + string(1, RECORD_SEPARATOR)
	);

	client.send(packed);

	sleep_for(std::chrono::milliseconds(100));

	// Expect read to return false due to invalid type.
	testing::internal::CaptureStdout();
	EXPECT_FALSE(server.read());
	string output = testing::internal::GetCapturedStdout();
	EXPECT_TRUE(output.find("Invalid message type received") != string::npos);
}

TEST_F(MessagesTest, ReadMalformedMessage) {
	Messages server(testPort);
	Socks client(LOCALHOST, testPort, false);

	// Malformed: no separators.
	string packed = "invalid";

	client.send(packed);

	sleep_for(std::chrono::milliseconds(100));

	// Expect read to return false.
	testing::internal::CaptureStdout();
	EXPECT_FALSE(server.read());
	string output = testing::internal::GetCapturedStdout();
	EXPECT_TRUE(output.find("Malformed message received") != string::npos);
}

TEST_F(MessagesTest, ReadEmptyBuffer) {
	Messages server(testPort);
	Socks client(LOCALHOST, testPort, false);

	// Send empty message.
	client.send("");

	sleep_for(std::chrono::milliseconds(100));

	EXPECT_FALSE(server.read());
}

TEST_F(MessagesTest, MultipleMessages) {
	Messages server(testPort);
	Socks client(LOCALHOST, testPort, false);

	// First valid message using Message class.
	Message msg1;
	msg1.setType(static_cast<Message::Types>(2));
	msg1.setFlags(3);
	msg1.addData("paramA");
	string packed1 = msg1.toString();

	client.send(packed1);

	sleep_for(std::chrono::milliseconds(100));

	EXPECT_TRUE(server.read());
	Message received1 = server.getMessage();
	EXPECT_EQ(received1.getType(), static_cast<Message::Types>(2));
	EXPECT_EQ(received1.getFlags(), 3);
	EXPECT_EQ(received1.getData().size(), 1);

	// Second valid message using Message class.
	Message msg2;
	msg2.setType(static_cast<Message::Types>(3));
	msg2.setFlags(7);
	msg2.addData("paramB1");
	msg2.addData("paramB2");
	string packed2 = msg2.toString();

	client.send(packed2);

	sleep_for(std::chrono::milliseconds(100));

	EXPECT_TRUE(server.read());
	Message received2 = server.getMessage();
	EXPECT_EQ(received2.getType(), static_cast<Message::Types>(3));
	EXPECT_EQ(received2.getFlags(), 7);
	EXPECT_EQ(received2.getData().size(), 2);

	// No more.
	EXPECT_FALSE(server.read());
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
