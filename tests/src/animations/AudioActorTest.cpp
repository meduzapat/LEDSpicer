/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      AudioActorTest.cpp
 * @since     Sep 11, 2025
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
#include "animations/AudioActor.hpp"

using namespace LEDSpicer::Animations;
using namespace LEDSpicer::Devices;
using namespace LEDSpicer::Utilities;

Color defaultColor;
vector<uint8_t> pins(10, 0);
vector<Element*> mockElements;

// Mock AudioActor subclass.
class MockAudioActor : public AudioActor {

public:

	MockAudioActor(unordered_map<string, string>& parameters, Group* const group) : AudioActor(parameters, group) {}

	void calcPeak() override {
		// Set mock values; can be overridden in tests.
		value.l = mockLeft;
		value.r = mockRight;
	}

	// Expose protected members for testing.
	using AudioActor::userPref;
	using AudioActor::totalElements;
	using AudioActor::colorData;
	using AudioActor::refreshPeak;
	using AudioActor::detectColor;
	using AudioActor::single;
	using AudioActor::vuMeters;
	using AudioActor::waves;
	using AudioActor::calculateElements;

	// Mock peaks.
	uint8_t mockLeft  = 0;
	uint8_t mockRight = 0;
};

class AudioActorTest : public ::testing::Test {

protected:

	void SetUp() override {
		// create 10 dummy elements.
		std::fill(pins.begin(), pins.end(), 0);
		for (uint8_t i = 0; i < 10; ++i) {
			auto e = new Element("mock" + to_string(i), &pins[i], defaultColor, 0, 100);
			mockElements.push_back(e);
		}
	}

	void TearDown() override {
		for (auto e : mockElements) delete e;
		mockElements.clear();
	}
};

TEST_F(AudioActorTest, Constructor) {

	StringUMap params = {
		{"mode",      "VuMeter"},
		{"off",       "Black"},
		{"low",       "Red"},
		{"mid",       "Yellow"},
		{"high",      "Green"},
		{"channel",   "Both"},
		{"direction", "Forward"}
	};

	Group group(defaultColor);
	// fill the group with mocks.
	for (uint8_t i = 0; i < 10; ++i) group.linkElement(mockElements.at(i));
	MockAudioActor actor(params, &group);
	EXPECT_EQ(actor.userPref.mode, AudioActor::Modes::VuMeter);
	EXPECT_EQ(actor.userPref.channel, AudioActor::Channels::Both);
	EXPECT_EQ(actor.userPref.off.getName(), "Black");
	EXPECT_EQ(actor.userPref.c00.getName(), "Red");
	EXPECT_EQ(actor.userPref.c50.getName(), "Yellow");
	EXPECT_EQ(actor.userPref.c75.getName(), "Green");
	EXPECT_EQ(actor.totalElements.l, 5);
	EXPECT_EQ(actor.totalElements.r, 5);
	EXPECT_EQ(actor.getDirection(), Direction::Directions::Forward);

	// Invalid mode fallback.
	StringUMap invalidParams = params;
	invalidParams["mode"] = "Invalid";
	string expected("Invalid mode Invalid assuming Single\n");
	testing::internal::CaptureStderr();
	MockAudioActor(invalidParams, &group);
	string output = testing::internal::GetCapturedStderr();
	EXPECT_EQ(expected, output);

	// Min elements check for VuMeter.
	Group smallGroup(defaultColor);
	// fill the group with mocks.
	for (uint8_t i = 0; i < 5; ++i) smallGroup.linkElement(mockElements.at(i));
	EXPECT_THROW(MockAudioActor(params, &smallGroup), Error);
}

TEST_F(AudioActorTest, DetectColor) {

	StringUMap params = {
		{"mode",    "Single"},
		{"off",     "Black"},
		{"low",     "Red"},
		{"mid",     "Yellow"},
		{"high",    "Green"},
		{"channel", "Mono"}
	};

	Group group(defaultColor);
	for (uint8_t i = 0; i < 10; ++i) group.linkElement(mockElements.at(i));
	MockAudioActor actor(params, &group);

	// No gradient.
	EXPECT_EQ(actor.detectColor(0,   false), Color::getColor("Black"));
	EXPECT_EQ(actor.detectColor(20,  false), Color::getColor("Red"));
	EXPECT_EQ(actor.detectColor(55,  false), Color::getColor("Yellow"));
	EXPECT_EQ(actor.detectColor(100, false), Color::getColor("Green"));
	EXPECT_EQ(actor.detectColor(96,  false), Color::getColor("Green"));

	// With gradient.
	EXPECT_EQ(actor.detectColor(0, true), Color::getColor("Black"));
	Color lowToMid = Color::getColor("Red").transition(Color::getColor("Yellow"), 50);  // 50% transition.
	EXPECT_EQ(actor.detectColor(40, true), lowToMid);
	EXPECT_EQ(actor.detectColor(100, true), Color::getColor("Green"));
}

int main(int argc, char **argv) {
	Color::loadColors({{"Green", "00FF00"}, {"Red", "FF0000"}, {"Yellow", "00FFFF"}, {"White", "FFFFFF"}, {"Black", "000000"}}, "hex");
	Log::setLogLevel(LOG_ERR);
	Log::logToStdTerm(true);
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}


