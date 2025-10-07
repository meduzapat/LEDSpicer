/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/**
 * @file ColorTest.cpp
 * @since Aug 21, 2025
 * @author Patricio A. Rossi (MeduZa)
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
#include "utilities/Color.hpp"

using namespace LEDSpicer::Utilities;

class TestColor : public Color, public ::testing::Test {

public:

	static void clearRandomColors() {
		randomColors.clear();
	}

	static void clearColors() {
		colors.clear();
		names.clear();
		clearRandomColors();
	}

	static size_t randomColorsSize() {
		return randomColors.size();
	}

	static const vector<const Color*>& getRandomColors() {
		return randomColors;
	}

protected:
	void SetUp() override {
		clearColors();
	}
};

TEST_F(TestColor, Constructors) {
	// Default constructor (black)
	Color c1;
	EXPECT_EQ(c1.getR(), 0);
	EXPECT_EQ(c1.getG(), 0);
	EXPECT_EQ(c1.getB(), 0);

	// RGB constructor
	Color c2(255, 128, 0);
	EXPECT_EQ(c2.getR(), 255);
	EXPECT_EQ(c2.getG(), 128);
	EXPECT_EQ(c2.getB(), 0);

	// String constructor (named color)
	Color::loadColors({{"Red", "FF0000"}}, "hex");
	Color c3("Red");
	EXPECT_EQ(c3.getR(), 255);
	EXPECT_EQ(c3.getG(), 0);
	EXPECT_EQ(c3.getB(), 0);

	// String with format (hex)
	Color c4("00FF00", "hex");
	EXPECT_EQ(c4.getR(), 0);
	EXPECT_EQ(c4.getG(), 255);
	EXPECT_EQ(c4.getB(), 0);

	// Invalid length sets to black
	Color c5("ABC", "hex");
	EXPECT_EQ(c5.getR(), 0);
	EXPECT_EQ(c5.getG(), 0);
	EXPECT_EQ(c5.getB(), 0);

	// Invalid hex chars sets to black
	Color c6("GGGGGG", "hex");
	EXPECT_EQ(c6.getR(), 0);
	EXPECT_EQ(c6.getG(), 0);
	EXPECT_EQ(c6.getB(), 0);
}

TEST_F(TestColor, Operators) {
	Color c1(100, 150, 200);
	Color c2(100, 150, 200);
	Color c3(101, 150, 200);

	EXPECT_TRUE(c1 == c2);
	EXPECT_FALSE(c1 == c3);
	EXPECT_FALSE(c1 != c2);
	EXPECT_TRUE(c1 != c3);
}

TEST_F(TestColor, SettersAndGetters) {
	Color c;

	// Set individual channels
	c.setR(10);
	c.setG(20);
	c.setB(30);
	EXPECT_EQ(c.getR(), 10);
	EXPECT_EQ(c.getG(), 20);
	EXPECT_EQ(c.getB(), 30);

	// Set RGB
	c.set(40, 50, 60);
	EXPECT_EQ(c.getR(), 40);
	EXPECT_EQ(c.getG(), 50);
	EXPECT_EQ(c.getB(), 60);

	// Set uint32_t (0xRRGGBB)
	c.set(0xAABBCC);
	EXPECT_EQ(c.getR(), 0xAA);
	EXPECT_EQ(c.getG(), 0xBB);
	EXPECT_EQ(c.getB(), 0xCC);

	// getRGB should match
	EXPECT_EQ(c.getRGB(), 0xAABBCC);

	// Set from Color
	Color c2(70, 80, 90);
	c.set(c2);
	EXPECT_EQ(c.getR(), 70);
	EXPECT_EQ(c.getG(), 80);
	EXPECT_EQ(c.getB(), 90);

	// Set from string (named)
	Color::loadColors({{"Blue", "0000FF"}}, "hex");
	c.set("Blue");
	EXPECT_EQ(c.getR(), 0);
	EXPECT_EQ(c.getG(), 0);
	EXPECT_EQ(c.getB(), 255);

	// Set from hex string
	c.set("FF00FF", true);
	EXPECT_EQ(c.getR(), 255);
	EXPECT_EQ(c.getG(), 0);
	EXPECT_EQ(c.getB(), 255);

	// Set with filter (Normal)
	Color c3(100, 100, 100);
	c3.set(Color(200, 200, 200), Color::Filters::Normal);
	EXPECT_EQ(c3.getR(), 200);

	// Set with filter (Combine, percent as uint8_t)
	c3.set(Color(100, 100, 100), Color::Filters::Combine, 50);
	EXPECT_EQ(c3.getR(), 150); // 200 + (100-200)*50/100 = 200 - 50 = 150

	// Set with filter (Mask, uses mono of new color, ignores percent)
	c3.set(Color(128, 128, 128), Color::Filters::Mask); // Mono ~128
	EXPECT_EQ(c3.getR(), 75); // 150 * 128 / 255 ≈ 75

	// Set with filter (Invert, inverts new color)
	c3.set(Color(50, 60, 70), Color::Filters::Invert);
	EXPECT_EQ(c3.getR(), 205); // 255-50
}

TEST_F(TestColor, Operations) {
	Color c1(0, 100, 200);

	// Fade
	Color faded = c1.fade(50.f);
	EXPECT_EQ(faded.getR(), 0);
	EXPECT_EQ(faded.getG(), 50);
	EXPECT_EQ(faded.getB(), 100);

	// Transition
	Color c2(255, 150, 50);
	Color trans = c1.transition(c2, 50.f);
	EXPECT_EQ(trans.getR(), 127); // 0 + (255-0)*50/100 ≈ 127
	EXPECT_EQ(trans.getG(), 125); // 100 + (150-100)*0.5 = 125
	EXPECT_EQ(trans.getB(), 125); // 200 + (50-200)*0.5 = 200 - 75 = 125

	// Difference (note: underflow wraps; suggest clamp in production)
	Color diff = c1.difference(c2);
	EXPECT_EQ(diff.getR(), 1); // 0-255 wraps to 1 in uint8_t
	EXPECT_EQ(diff.getG(), 206); // 100-150 wraps to 206
	EXPECT_EQ(diff.getB(), 150); // 200-50 = 150

	// Mask
	Color masked = c1.mask(128.f); // ~50%
	EXPECT_EQ(masked.getR(), 0);
	EXPECT_EQ(masked.getG(), 50); // 100*128/255≈50
	EXPECT_EQ(masked.getB(), 100); // 200*128/255≈100

	// Invert
	Color inv = c1.invert();
	EXPECT_EQ(inv.getR(), 255);
	EXPECT_EQ(inv.getG(), 155);
	EXPECT_EQ(inv.getB(), 55);

	// Monochrome
	EXPECT_EQ(c1.getMonochrome(), static_cast<uint8_t>(0*0.301f + 100*0.587f + 200*0.114f)); // ≈81
}

TEST_F(TestColor, StaticMethods) {
	// loadColors (hex, skips Random)
	Color::loadColors({{"Green", "00FF00"}, {"Blue", "0000FF"}}, "hex");
	EXPECT_TRUE(Color::hasColor("Green"));
	EXPECT_TRUE(Color::hasColor("Blue"));
	EXPECT_FALSE(Color::hasColor("Random"));

	// getColor
	Color green = Color::getColor("Green");
	EXPECT_EQ(green.getR(), 0);
	EXPECT_EQ(green.getG(), 255);

	// Specials
	EXPECT_EQ(Color::getColor("On").getR(), 255);
	EXPECT_EQ(Color::getColor("Off").getR(), 0);

	// filter2str and str2filter
	EXPECT_EQ(Color::filter2str(Color::Filters::Mask), "Mask");
	EXPECT_EQ(Color::str2filter("Invert"), Color::Filters::Invert);
	EXPECT_THROW(Color::str2filter("Invalid"), Error);

	// getNames
	const vector<string>& names = Color::getNames();
	EXPECT_TRUE(std::find(names.begin(), names.end(), "Green") != names.end());
	EXPECT_TRUE(std::find(names.begin(), names.end(), "Blue") != names.end());

	// getName
	EXPECT_EQ(green.getName(), "Green");
	EXPECT_EQ(Color::On.getName(), "On");
	EXPECT_EQ(Color(1,2,3).getName(), "unknown");

	// Random (set specific)
	Color::setRandomColors({"Green"});
	EXPECT_EQ(TestColor::randomColorsSize(), 1);
	EXPECT_EQ(TestColor::getRandomColors()[0]->getName(), "Green");
	Color rand = Color::getColor("Random");
	EXPECT_EQ(rand.getName(), "Green");

	// Random (set all)
	TestColor::clearRandomColors();
	Color::setRandomColors({});
	EXPECT_EQ(TestColor::randomColorsSize(), 2);
	EXPECT_EQ(TestColor::getRandomColors()[0]->getName(), "Green");
	EXPECT_EQ(TestColor::getRandomColors()[1]->getName(), "Blue");

	// setRandomColors with unknown
	TestColor::clearRandomColors();
	EXPECT_THROW(Color::setRandomColors({"Unknown"}), Error);

	// Test multiple calls do nothing
	Color::setRandomColors({});
	size_t prevSize = TestColor::randomColorsSize();
	Color::setRandomColors({"Green"});
	EXPECT_EQ(TestColor::randomColorsSize(), prevSize); // Unchanged
}

TEST_F(TestColor, RandomCannotSetRandomCases) {
	EXPECT_THROW(Color::setRandomColors({"Random", "Random", "Random"}), Error);
}

TEST_F(TestColor, EdgeCasesAndErrors) {
	Color::loadColors({{"White", "FFFFFF"}}, "hex");

	// Invalid color
	EXPECT_THROW(Color::getColor("Unknown"), Error);

	// transition extremes
	Color c(0,0,0);
	EXPECT_EQ(c.transition(Color(255,255,255), 0.f).getR(), 0); // <1 returns this
	EXPECT_EQ(c.transition(Color(255,255,255), 100.f).getR(), 255); // >99 returns dest

	// mask extremes
	EXPECT_EQ(c.mask(0.f).getR(), 0);
	EXPECT_EQ(c.mask(255.f).getR(), 0); // For black base

	// Percent >100 in combine (treats as >99, returns dest)
	Color c4(0,0,0);
	c4.set(Color(255,255,255), Color::Filters::Combine, 200);
	EXPECT_EQ(c4.getR(), 255);
}

// Note: draw methods not tested (require cout capture).

// Main function for running tests
int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
