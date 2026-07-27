/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      UltimateTest.cpp
 * @since     Jul 27, 2026
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

/*
 * The source, not the header: deviceFactory() defines createDevice and destroyDevice in every
 * translation unit that includes Ultimate.hpp, so the board has to be built into this one.
 */
#include "devices/Ultimarc/Ultimate.cpp"

using namespace LEDSpicer::Devices::Ultimarc;
using namespace LEDSpicer::Utilities;

/// Exposes the parts of the board that only the connection sequence would reach.
class TestUltimate : public Ultimate {
public:
	TestUltimate(StringUMap& options) : Ultimate(options) {}

	using Ultimate::matchesSignature;
	using Ultimate::notFoundHint;
	using Ultimate::connect;
	using Ultimate::disconnect;
	using Ultimate::afterConnect;
	using Ultimate::afterClaimInterface;
	using Ultimate::LEDs;

	uint8_t getInterface() const { return interface; }

	bool isFast() const { return hasFastLedReport; }
};

/// A board descriptor with the product code the mode and the board position produce.
libusb_device* board(uint16_t product, uint8_t interfaces = 3, uint8_t usagePage = 1) {

	static const uint8_t hidDescriptor[] {
		9, HID_DESCRIPTOR, 0x11, 0x01, 0, 1, HID_REPORT_DESCRIPTOR, 4, 0
	};
	static libusb_interface_descriptor altsettings[4];
	static libusb_interface           interfaceList[4];
	static libusb_config_descriptor   configuration;

	libusb_device* device = new libusb_device();
	device->descriptor.idVendor  = ULTIMARC_VENDOR;
	device->descriptor.idProduct = product;

	for (uint8_t c = 0; c < interfaces; ++c) {
		altsettings[c].bInterfaceNumber = c;
		altsettings[c].extra            = hidDescriptor;
		altsettings[c].extra_length     = sizeof(hidDescriptor);
		interfaceList[c].altsetting     = &altsettings[c];
	}
	configuration.bNumInterfaces = interfaces;
	configuration.interface      = interfaceList;
	device->configuration        = &configuration;

	// Usage Page (one byte of data), then an unrelated two byte item.
	device->reportDescriptor = {HID_ITEM_USAGE_PAGE | 1, usagePage, 0x0A, 0, 0};

	return device;
}

class UltimateTest : public ::testing::Test {

protected:

	StringUMap options;

	void SetUp() override {
		fakeFailInit = false;
		fakeFailOpen = false;
		for (auto device : fakeDevices) delete device;
		fakeDevices.clear();
		fakeTransfers.clear();
		options.clear();
		Log::logToStdTerm(true);
		Log::setLogLevel(LOG_ERR);
	}

	void TearDown() override {
		USB::closeSession();
	}
};

TEST_F(UltimateTest, MatchesBoardOneInEitherMode) {

	options["boardId"] = "1";
	TestUltimate ultimate(options);

	libusb_device_descriptor descriptor;
	descriptor.idVendor = ULTIMARC_VENDOR;

	// Board 1 keyboard, board 1 Dinput.
	descriptor.idProduct = 0x0410;
	EXPECT_TRUE(ultimate.matchesSignature(descriptor));
	descriptor.idProduct = 0x0412;
	EXPECT_TRUE(ultimate.matchesSignature(descriptor));

	// Board 2 in either mode is not board 1.
	descriptor.idProduct = 0x0411;
	EXPECT_FALSE(ultimate.matchesSignature(descriptor));
	descriptor.idProduct = 0x0413;
	EXPECT_FALSE(ultimate.matchesSignature(descriptor));
}

TEST_F(UltimateTest, MatchesBoardTwoInEitherMode) {

	options["boardId"] = "2";
	TestUltimate ultimate(options);

	libusb_device_descriptor descriptor;
	descriptor.idVendor = ULTIMARC_VENDOR;

	descriptor.idProduct = 0x0411;
	EXPECT_TRUE(ultimate.matchesSignature(descriptor));
	descriptor.idProduct = 0x0413;
	EXPECT_TRUE(ultimate.matchesSignature(descriptor));

	descriptor.idProduct = 0x0410;
	EXPECT_FALSE(ultimate.matchesSignature(descriptor));
	descriptor.idProduct = 0x0412;
	EXPECT_FALSE(ultimate.matchesSignature(descriptor));
}

TEST_F(UltimateTest, RejectsForeignDevices) {

	options["boardId"] = "1";
	TestUltimate ultimate(options);

	libusb_device_descriptor descriptor;

	// The Xbox 360 controller the board turns into under Xinput.
	descriptor.idVendor  = 0x045E;
	descriptor.idProduct = 0x028E;
	EXPECT_FALSE(ultimate.matchesSignature(descriptor));

	// Another Ultimarc board, outside the block.
	descriptor.idVendor  = ULTIMARC_VENDOR;
	descriptor.idProduct = 0x1401;
	EXPECT_FALSE(ultimate.matchesSignature(descriptor));

	// Just past the end of the block.
	descriptor.idProduct = 0x0414;
	EXPECT_FALSE(ultimate.matchesSignature(descriptor));
}

TEST_F(UltimateTest, RejectsBoardIdBeyondTheHardware) {
	// The two board block leaves no room for the boardId 3 workaround from issue 16.
	options["boardId"] = "3";
	EXPECT_THROW({TestUltimate board(options);}, Error);
}

TEST_F(UltimateTest, XinputHintOnFailureToFind) {

	options["boardId"] = "1";
	TestUltimate ultimate(options);

	try {
		ultimate.connect();
		FAIL() << "connect() should not find a board";
	}
	catch (Error& e) {
		EXPECT_NE(e.getMessage().find("Xinput"), string::npos);
	}
}

TEST_F(UltimateTest, KeyboardModeUsesTheThirdInterface) {

	options["boardId"] = "1";
	TestUltimate ultimate(options);
	fakeDevices.push_back(board(0x0410, 3));

	ultimate.connect();
	ultimate.afterConnect();
	EXPECT_EQ(ultimate.getInterface(), 2);
	ultimate.disconnect();
}

TEST_F(UltimateTest, DinputModeUsesTheFourthInterface) {

	// The regression from issue 16: the gamepad adds an interface and the LEDs move behind it.
	options["boardId"] = "1";
	TestUltimate ultimate(options);
	fakeDevices.push_back(board(0x0412, 4));

	ultimate.connect();
	ultimate.afterConnect();
	EXPECT_EQ(ultimate.getInterface(), 3);
	ultimate.disconnect();
}

TEST_F(UltimateTest, DetectsTheFastReportFromTheUsagePage) {

	options["boardId"] = "1";
	TestUltimate fast(options);
	fakeDevices.push_back(board(0x0410, 3, IPAC_ULTIMATE_FAST_PAGE));
	fast.connect();
	fast.afterConnect();
	fast.afterClaimInterface();
	EXPECT_TRUE(fast.isFast());
	fast.disconnect();

	for (auto device : fakeDevices) delete device;
	fakeDevices.clear();

	TestUltimate paired(options);
	fakeDevices.push_back(board(0x0410, 3, 1));
	paired.connect();
	paired.afterConnect();
	paired.afterClaimInterface();
	EXPECT_FALSE(paired.isFast());
	paired.disconnect();
}

TEST_F(UltimateTest, FastReportSendsEverythingInOnePacket) {

	options["boardId"] = "1";
	TestUltimate ultimate(options);
	fakeDevices.push_back(board(0x0410, 3, IPAC_ULTIMATE_FAST_PAGE));

	ultimate.connect();
	ultimate.afterConnect();
	ultimate.afterClaimInterface();
	ultimate.setLed(0, 0x11);
	ultimate.setLed(1, 0x22);

	fakeTransfers.clear();
	ultimate.transfer();

	ASSERT_EQ(fakeTransfers.size(), 1);
	ASSERT_EQ(fakeTransfers[0].data.size(), IPAC_ULTIMATE_LEDS + 1);
	EXPECT_EQ(fakeTransfers[0].data[0], IPAC_ULTIMATE_FAST_REPORT);
	EXPECT_EQ(fakeTransfers[0].data[1], 0x11);
	EXPECT_EQ(fakeTransfers[0].data[2], 0x22);
	ultimate.disconnect();
}

TEST_F(UltimateTest, PairedReportStreamsFortyEightPairsAfterAReset) {

	options["boardId"] = "1";
	TestUltimate ultimate(options);
	fakeDevices.push_back(board(0x0410, 3, 1));

	ultimate.connect();
	ultimate.afterConnect();
	ultimate.afterClaimInterface();
	ultimate.setLed(0, 0x11);
	ultimate.setLed(1, 0x22);

	fakeTransfers.clear();
	ultimate.transfer();

	// One reset plus one message per pair.
	ASSERT_EQ(fakeTransfers.size(), 1 + IPAC_ULTIMATE_LEDS / 2);

	const vector<uint8_t> reset {0x03, 0xFE, 0, 0, 0}, first {0x03, 0x11, 0x22, 0, 0};
	EXPECT_EQ(fakeTransfers[0].data, reset);
	EXPECT_EQ(fakeTransfers[1].data, first);
	ultimate.disconnect();
}

TEST_F(UltimateTest, ResetLedsCarriesTheCommandInTheSecondByte) {

	options["boardId"] = "1";
	TestUltimate ultimate(options);
	fakeDevices.push_back(board(0x0410, 3, IPAC_ULTIMATE_FAST_PAGE));

	ultimate.connect();
	ultimate.afterConnect();
	ultimate.afterClaimInterface();

	fakeTransfers.clear();
	ultimate.resetLeds();

	// Off ramp speed, then every LED to intensity zero.
	const vector<uint8_t> ramp {0x03, 0xC0, 0, 0, 0}, off {0x03, 0x80, 0, 0, 0};
	ASSERT_EQ(fakeTransfers.size(), 2);
	EXPECT_EQ(fakeTransfers[0].data, ramp);
	EXPECT_EQ(fakeTransfers[1].data, off);
	ultimate.disconnect();
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
