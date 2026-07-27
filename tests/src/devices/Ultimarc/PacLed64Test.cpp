/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      PacLed64Test.cpp
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
 * translation unit that includes PacLed64.hpp, so the board has to be built into this one.
 */
#include "devices/Ultimarc/PacLed64.cpp"

using namespace LEDSpicer::Devices::Ultimarc;
using namespace LEDSpicer::Utilities;

/// Guards the wire format of the pair stream that moved to Ultimarc::transferPairs().
class TestPacLed64 : public PacLed64 {
public:
	TestPacLed64(StringUMap& options) : PacLed64(options) {}

	using PacLed64::connect;
	using PacLed64::disconnect;
};

class PacLed64Test : public ::testing::Test {

protected:

	StringUMap options;

	void SetUp() override {
		fakeFailInit = false;
		fakeFailOpen = false;
		for (auto device : fakeDevices) delete device;
		fakeDevices.clear();
		fakeTransfers.clear();
		options.clear();
		options["boardId"] = "1";

		libusb_device* device = new libusb_device();
		device->descriptor.idVendor  = ULTIMARC_VENDOR;
		device->descriptor.idProduct = PAC_LED64_PRODUCT;
		fakeDevices.push_back(device);

		Log::logToStdTerm(true);
		Log::setLogLevel(LOG_ERR);
	}

	void TearDown() override {
		USB::closeSession();
	}
};

TEST_F(PacLed64Test, TransferStreamsPairsAfterAReset) {

	TestPacLed64 board(options);
	board.connect();
	board.setLed(0, 0x11);
	board.setLed(1, 0x22);
	board.setLed(2, 0x33);

	fakeTransfers.clear();
	board.transfer();

	// One reset plus one two byte message per pair, unchanged from before the helper moved.
	ASSERT_EQ(fakeTransfers.size(), 1 + PAC_LED64_LEDS / 2);

	const vector<uint8_t> reset {0xFE, 0}, first {0x11, 0x22}, second {0x33, 0};
	EXPECT_EQ(fakeTransfers[0].data, reset);
	EXPECT_EQ(fakeTransfers[1].data, first);
	EXPECT_EQ(fakeTransfers[2].data, second);
	board.disconnect();
}

TEST_F(PacLed64Test, ResetLedsSetsTheRampThenEveryLedOff) {

	TestPacLed64 board(options);
	board.connect();

	fakeTransfers.clear();
	board.resetLeds();

	const vector<uint8_t> ramp {0xC0, 0}, off {0x80, 0};
	ASSERT_EQ(fakeTransfers.size(), 2);
	EXPECT_EQ(fakeTransfers[0].data, ramp);
	EXPECT_EQ(fakeTransfers[1].data, off);
	board.disconnect();
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
