/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      GroupTest.cpp
 * @since     Aug 31, 2025
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
#include "devices/Group.hpp"

using namespace LEDSpicer::Devices;
using LEDSpicer::Utilities::Color;

TEST(GroupTest, ConstructorWithName) {
	Group group("TestGroup", Color::On);
	EXPECT_EQ(group.getName(), "TestGroup");
	EXPECT_EQ(group.getDefaultColor().getR(), 255);
	EXPECT_EQ(group.getDefaultColor().getG(), 255);
	EXPECT_EQ(group.getDefaultColor().getB(), 255);
	EXPECT_EQ(group.size(), 0);
}

TEST(GroupTest, ConstructorWithoutName) {
	Color c(128, 128, 128);
	Group group(c);
	EXPECT_EQ(group.getName(), "");
	EXPECT_EQ(group.getDefaultColor().getR(), 128);
	EXPECT_EQ(group.getDefaultColor().getG(), 128);
	EXPECT_EQ(group.getDefaultColor().getB(), 128);
	EXPECT_EQ(group.size(), 0);
}

TEST(GroupTest, LinkElement) {
	Group group("TestGroup", Color::Off);
	uint8_t ledValue = 0;
	Element element("TestElement", &ledValue, Color::Off, 0, 100);
	group.linkElement(&element);
	EXPECT_EQ(group.size(), 1);
	EXPECT_EQ(group.getElement(0), &element);
	EXPECT_EQ(group.getElements().size(), 1);
	EXPECT_EQ(group.getElements()[0]->getName(), "TestElement");
}

TEST(GroupTest, getLeds) {
	Group group("TestGroup", Color::Off);
	uint8_t
		led1 = 1,
		led2 = 2,
		led3 = 0;
	Element element1("Element1", &led1, Color::Off, 0, 100);
	Element element2("Element2", &led2, Color::Off, 0, 100);
	Element solenoid("Solenoid", &led3, Color::Off, 2, 100);
	group.linkElement(&element1);
	group.linkElement(&element2);
	group.linkElement(&solenoid);
	// Solenoids are ignored
	EXPECT_EQ(group.getLeds().size(), 2);
	EXPECT_EQ(*group.getLeds()[0], 1);
	EXPECT_EQ(*group.getLeds()[1], 2);
}


TEST(GroupTest, GetElementOutOfRange) {
	// This method was never used and it should throw.
	Group group("TestGroup", Color::Off);
	EXPECT_THROW(group.getElement(0), std::out_of_range);
}

TEST(GroupTest, ShrinkToFit) {
	Group group("TestGroup", Color::Off);
	uint8_t ledValue = 0;
	Element element("TestElement", &ledValue, Color::Off, 0, 100);
	group.linkElement(&element);
	group.shrinkToFit();
	EXPECT_EQ(group.size(), 1);
	EXPECT_EQ(group.getElement(0), &element);
}

TEST(GroupTest, MultipleElements) {
	Group group("TestGroup", Color::Off);
	uint8_t
		led1 = 0,
		led2 = 0;
	Element element1("Element1", &led1, Color::Off, 0, 100);
	Element element2("Element2", &led2, Color::Off, 0, 100);
	group.linkElement(&element1);
	group.linkElement(&element2);
	EXPECT_EQ(group.size(), 2);
	EXPECT_EQ(group.getElement(0), &element1);
	EXPECT_EQ(group.getElement(1), &element2);
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
