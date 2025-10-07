/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      USBTest.cpp
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

#include "utilities/USB.hpp"

using namespace LEDSpicer::Utilities;

// Mock subclass for pure virtuals (override in real devices).
class MockUSB : public USB {
public:
	MockUSB(uint16_t wValue, uint8_t interface, uint8_t boardId, uint8_t maxBoards) :
		USB(wValue, interface, boardId, maxBoards) {}

	// Expose protected base members as public for testing.
	using USB::connect;
	using USB::disconnect;
	using USB::claimInterface;
	using USB::transferToConnection;
	using USB::transferFromConnection;

	virtual uint16_t getVendor() const override { return 0x1234; }
	virtual uint16_t getProduct() const override { return 0xABCD; }
	virtual void afterConnect() override {}
	virtual void afterClaimInterface() override {}
};

class USBTest : public ::testing::Test {

protected:

	void SetUp() override {
		// Reset fake globals for each test.
		fakeFailInit = false;
		fakeFailOpen = false;
		for (auto dev : fakeDevices) delete dev;
		fakeDevices.clear();
		Log::logToStdTerm(true);
		Log::setLogLevel(LOG_ERR);
	}

	void TearDown() override {
		// Clean up any lingering session (though fakes handle deletion).
		USB::closeSession();
	}
};

TEST_F(USBTest, ConstructorAndGetters) {
	MockUSB usb(0x0200, 0, 1, 5);
	EXPECT_EQ(usb.getId(), 1);
	EXPECT_TRUE(usb.isProductBasedId());
	EXPECT_FALSE(usb.isNonBasedId());

	// Invalid board ID throws.
	EXPECT_THROW(MockUSB(0x0200, 0, 0, 5), Error);
	EXPECT_THROW(MockUSB(0x0200, 0, 6, 5), Error);
	usb.disconnect();
}

TEST_F(USBTest, InitFailure) {
	fakeFailInit = true;
	// Failed to initialize so no need for disconnect.
	EXPECT_THROW(MockUSB(0x0200, 0, 1, 5), Error);
}

TEST_F(USBTest, ConnectDisconnect) {
	// Interface 1, board ID 2.
	MockUSB usb(0x0200, 1, 2, 5);
	libusb_device* fakeDev = new libusb_device();
	fakeDevices.push_back(fakeDev);

	// Capture logs to verify.
	Log::setLogLevel(LOG_DEBUG);
	testing::internal::CaptureStdout();
	usb.connect();
	usb.claimInterface();
	string output = testing::internal::GetCapturedStdout();
	EXPECT_TRUE(output.find("Connecting to 0x1234:0xabcd Id: 2") != string::npos);
	EXPECT_TRUE(output.find("Claiming interface 1") != string::npos);

	// Disconnect.
	testing::internal::CaptureStdout();
	usb.disconnect();
	output = testing::internal::GetCapturedStdout();
	EXPECT_TRUE(output.find("Reseting interface: 1") != string::npos);
	EXPECT_TRUE(output.find("Closing interface: 1") != string::npos);
	// To avoid closing debug logs output.
	Log::setLogLevel(LOG_NOTICE);
}

TEST_F(USBTest, ConnectNoDevice) {
	MockUSB usb(0x0200, 0, 1, 5);
	EXPECT_THROW(usb.connect(), Error);
}

TEST_F(USBTest, ConnectOpenFailure) {
	libusb_device* fakeDev = new libusb_device();
	fakeDevices.push_back(fakeDev);
	fakeFailOpen = true;
	MockUSB usb(0x0200, 0, 1, 5);
	EXPECT_THROW(usb.connect(), Error);
}

TEST_F(USBTest, TransferToConnection) {
	MockUSB usb(0x0200, 0, 1, 5);

	// Fake a device and connect.
	libusb_device* fakeDev = new libusb_device();
	fakeDevices.push_back(fakeDev);
	usb.connect();
	usb.claimInterface();

	vector<uint8_t> data = {0x01, 0x02, 0x03};
	Log::setLogLevel(LOG_DEBUG);
	testing::internal::CaptureStdout();
	usb.transferToConnection(data);
	string output = testing::internal::GetCapturedStdout();
	EXPECT_TRUE(output.find("Fake Transfer: wValue: 0x200 wIndex: 0x0 wLength: 3") != string::npos);
	Log::setLogLevel(LOG_NOTICE);
	usb.disconnect();
}

TEST_F(USBTest, TransferFromConnection) {
	//NOTE: This test is not complete because USB::transferFromConnection is not implemented.
	MockUSB usb(0x0200, 0, 1, 5);
	libusb_device* fakeDev = new libusb_device();
	fakeDevices.push_back(fakeDev);
	usb.connect();
	usb.claimInterface();
	EXPECT_EQ(usb.transferFromConnection(3).size(), 3);
}

TEST_F(USBTest, CloseSession) {
	MockUSB usb(0x0200, 0, 1, 5);
	Log::setLogLevel(LOG_DEBUG);
	testing::internal::CaptureStdout();
	USB::closeSession();
	string output = testing::internal::GetCapturedStdout();
	EXPECT_TRUE(output.find("Closing USB session") != string::npos);
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
