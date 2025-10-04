/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      SocksTest.cpp
 * @since     Aug 22, 2025
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
#include "utilities/Socks.hpp"

using namespace LEDSpicer::Utilities;

TEST(SocksTest, DefaultConstructor) {
	Socks s;
	EXPECT_FALSE(s.isConnected());
}

TEST(SocksTest, PrepareInvalidHost) {
	Socks s;
	EXPECT_THROW(s.prepare("invalid_host", "12345", false), Error);
}

TEST(SocksTest, PrepareInvalidPort) {
	Socks s;
	EXPECT_THROW(s.prepare("127.0.0.1", "invalid_port", false), Error);
}

TEST(SocksTest, ClientServerTransmission) {
	// Use a high port to avoid conflicts.
	const string port = "54321";
	const string message = "Test message";
	string received;

	// Server: bind.
	Socks server;
	ASSERT_NO_THROW(server.prepare("127.0.0.1", port, true));
	EXPECT_TRUE(server.isConnected());

	// Client: connect.
	Socks client;
	ASSERT_NO_THROW(client.prepare("127.0.0.1", port, false));
	EXPECT_TRUE(client.isConnected());

	// Send from client.
	EXPECT_TRUE(client.send(message));

	// Small delay for non-blocking receive.
	sleep_for(std::chrono::milliseconds(100));

	// Receive on server.
	EXPECT_TRUE(server.receive(received));
	EXPECT_EQ(received, message);

	// Empty receive should return false.
	EXPECT_FALSE(server.receive(received));

	// Disconnect.
	client.disconnect();
	server.disconnect();
	EXPECT_FALSE(client.isConnected());
	EXPECT_FALSE(server.isConnected());
}

TEST(SocksTest, SendEmptyMessage) {
	Socks s;
	ASSERT_NO_THROW(s.prepare("127.0.0.1", "54322", false));
	EXPECT_TRUE(s.send(""));
}

TEST(SocksTest, SendLargeMessage) {

	const string port = "54321";
	const string message(BUFFER_SIZE * 5, 'A'); // bigger than BUFFER_SIZE (1024)
	string received;

	// Server: bind.
	Socks server;
	ASSERT_NO_THROW(server.prepare("127.0.0.1", port, true));
	EXPECT_TRUE(server.isConnected());

	// Client: connect.
	Socks client;
	ASSERT_NO_THROW(client.prepare("127.0.0.1", port, false));
	EXPECT_TRUE(client.isConnected());

	// Send from client.
	EXPECT_TRUE(client.send(message));

	// Small delay for non-blocking receive.
	sleep_for(std::chrono::milliseconds(100));

	// Receive on server.
	EXPECT_TRUE(server.receive(received));
	EXPECT_EQ(received, message);

	// Disconnect.
	client.disconnect();
	server.disconnect();
}

TEST(SocksTest, ReceiveOnDisconnected) {
	Socks s;
	string buffer;
	EXPECT_FALSE(s.receive(buffer));
}

TEST(SocksTest, DestructorDisconnects) {
	string port = "54323";
	{
		Socks s;
		s.prepare("127.0.0.1", port, false);
		EXPECT_TRUE(s.isConnected());
	} // Destructor called.
	// Attempt to bind on same port should succeed if disconnected.
	Socks s2;
	EXPECT_NO_THROW(s2.prepare("127.0.0.1", port, true));
	s2.disconnect();
}

// Main function for running tests
int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
