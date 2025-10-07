/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      ElementTest.cpp
 * @since     Aug 30, 2025
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
#include "devices/Element.hpp"

using namespace LEDSpicer::Devices;
using LEDSpicer::Utilities::Color;

TEST(ElementTest, MonochromeConstructor) {
	uint8_t ledValue = 0;
	Element element("MonoTest", &ledValue, Color::On, 0, 100);
	EXPECT_EQ(element.getName(), "MonoTest");
	EXPECT_EQ(element.size(), 1);
	EXPECT_EQ(element.getDefaultColor().getR(), 255);
	EXPECT_EQ(element.getDefaultColor().getG(), 255);
	EXPECT_EQ(element.getDefaultColor().getB(), 255);
	EXPECT_FALSE(element.isTimed());
}

TEST(ElementTest, SolenoidConstructor) {
	uint8_t ledValue = 0;
	Element element("MonoTest", &ledValue, Color::Off, 50, 100);
	EXPECT_TRUE(element.isTimed());
	element.setColor(Color::On);
	EXPECT_EQ(element.getLedValue(0), 255);
	sleep_for(std::chrono::milliseconds(51));
	element.checkTime();
	EXPECT_EQ(element.getLedValue(0), 0);
}

TEST(ElementTest, RGBConstructor) {
	uint8_t
		ledR = 0,
		ledG = 0,
		ledB = 0;
	Color col(128, 128, 128);
	Element element("RGBTest", &ledR, &ledG, &ledB, col, 50);
	EXPECT_EQ(element.getName(), "RGBTest");
	EXPECT_EQ(element.size(), 3);
	EXPECT_EQ(element.getDefaultColor().getR(), 128);
	EXPECT_EQ(element.getDefaultColor().getG(), 128);
	EXPECT_EQ(element.getDefaultColor().getB(), 128);
	EXPECT_FALSE(element.isTimed());
}

TEST(ElementTest, MultiRGBConstructor) {
	vector<uint8_t*> leds = {new uint8_t(0), new uint8_t(0), new uint8_t(0), new uint8_t(0), new uint8_t(0), new uint8_t(0)};
	Color col(64, 64, 64);
	Element element("MultiRGBTest", leds, col, 75);
	EXPECT_EQ(element.getName(), "MultiRGBTest");
	EXPECT_EQ(element.size(), 6);
	EXPECT_EQ(element.getDefaultColor().getR(), 64);
	EXPECT_FALSE(element.isTimed());
	for (uint8_t* led : leds) delete led;
}

TEST(ElementTest, SetColorMonochrome) {
	uint8_t ledValue = 0;
	Element element("MonoTest", &ledValue, Color::Off, 0, 100);
	element.setColor(Color(100, 150, 200));
	// lumina formula Y=0.301⋅r+0.587⋅g+0.114⋅b = 140
	EXPECT_EQ(element.getLedValue(0), 140);
}

TEST(ElementTest, SetColorRGB) {
	uint8_t
		ledR = 0,
		ledG = 0,
		ledB = 0;
	Element element("RGBTest", &ledR, &ledG, &ledB, Color::On, 100);
	element.setColor(Color(255, 128, 64));
	EXPECT_EQ(element.getLedValue(Color::Channels::Red), 255);
	EXPECT_EQ(element.getLedValue(Color::Channels::Green), 128);
	EXPECT_EQ(element.getLedValue(Color::Channels::Blue), 64);
}

TEST(ElementTest, SetColorMultiRGB) {
	vector<uint8_t*> leds = {new uint8_t(0), new uint8_t(0), new uint8_t(0), new uint8_t(0), new uint8_t(0), new uint8_t(0)};
	Element element("MultiRGBTest", leds, Color::On, 100);
	element.setColor(Color(200, 100, 50));
	for (size_t i = 0; i < leds.size(); i += 3) {
		EXPECT_EQ(*leds[i + Color::Channels::Red], 200);
		EXPECT_EQ(*leds[i + Color::Channels::Green], 100);
		EXPECT_EQ(*leds[i + Color::Channels::Blue], 50);
	}
	for (uint8_t* led : leds) {
		delete led;
	}
}

TEST(ElementTest, SetColorWithBrightness) {
	uint8_t
		ledR = 0,
		ledG = 0,
		ledB = 0;
	Element element("BrightnessTest", &ledR, &ledG, &ledB, Color::On, 50); // 50% brightness
	element.setColor(Color(200, 100, 50));
	EXPECT_EQ(element.getLedValue(Color::Channels::Red), 100);   // 200 * 0.5
	EXPECT_EQ(element.getLedValue(Color::Channels::Green), 50);  // 100 * 0.5
	EXPECT_EQ(element.getLedValue(Color::Channels::Blue), 25);   // 50 * 0.5
}

TEST(ElementTest, SetColorWithFilter) {
	uint8_t
		ledR = 0,
		ledG = 0,
		ledB = 0;
	Element element("FilterTest", &ledR, &ledG, &ledB, Color::On, 100);
	element.setColor(Color(200, 100, 50), Color::Filters::Combine, 50);
	Color currentColor = element.getColor();
	EXPECT_EQ(currentColor.getR(), 100); // Additive filter at 50% blends with existing (0 + 200 * 0.5)
	EXPECT_EQ(currentColor.getG(), 50);
	EXPECT_EQ(currentColor.getB(), 25);
}

TEST(ElementTest, GetColor) {
	uint8_t
		ledR = 200,
		ledG = 100,
		ledB = 50;
	Element element("ColorTest", &ledR, &ledG, &ledB, Color::On, 100);
	Color color = element.getColor();
	EXPECT_EQ(color.getR(), 200);
	EXPECT_EQ(color.getG(), 100);
	EXPECT_EQ(color.getB(), 50);
}

TEST(ElementTest, GetLed) {
	uint8_t ledValue = 42;
	Element element("LedTest", &ledValue, Color::On, 0, 100);
	EXPECT_EQ(*element.getLed(0), 42);
	EXPECT_THROW(element.getLed(1), Error);
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
