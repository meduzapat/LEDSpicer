/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      ItemsTest.cpp
 * @since     Sep 2, 2025
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
#include "devices/Element.hpp"
#include "devices/Group.hpp"

using namespace LEDSpicer::Devices;
using LEDSpicer::Utilities::Color;

TEST(ItemsTest, Constructor) {
	Color color(255, 128, 64);
	Element element(
		"TestElement",
		nullptr,
		color,
		0,
		100
	);
	Items* item = new Element::Item(
		&element,
		&color,
		Color::Filters::Normal,
		42
	);
	EXPECT_EQ(item->color, &color);
	EXPECT_EQ(item->filter, Color::Filters::Normal);
	EXPECT_EQ(item->pos, 42);
	delete item;
}

TEST(ItemsTest, ElementItem) {
	Color defaultColor(0, 0, 0);
	uint8_t ledValue = 0;
	Element element(
		"TestElement",
		&ledValue,
		defaultColor,
		0,
		100
	);
	Color color(255, 128, 64);
	Element::Item item(
		&element,
		&color,
		Color::Filters::Normal,
		10
	);
	EXPECT_EQ(item.getName(), "TestElement");
	item.process(50, nullptr);
	EXPECT_EQ(ledValue, 159);
}

TEST(ItemsTest, ElementItemWithFilter) {
	Color defaultColor(0, 0, 0);
	uint8_t ledValue = 100;
	Element element(
		"TestElement",
		&ledValue,
		defaultColor,
		0,
		100
	);
	Color color(255, 128, 64);
	Element::Item item(
		&element,
		&color,
		Color::Filters::Combine,
		10
	);
	item.process(50, nullptr);
	EXPECT_EQ(ledValue, 129);
}

TEST(ItemsTest, GroupItem) {
	Color defaultColor(0, 0, 0);
	Group group(
		"TestGroup",
		defaultColor
	);
	uint8_t
		ledValue1 = 0,
		ledValue2 = 0;
	Element element1(
		"Element1",
		&ledValue1,
		defaultColor,
		0,
		100
	);
	Element element2(
		"Element2",
		&ledValue2,
		defaultColor,
		0,
		100
	);
	group.linkElement(&element1);
	group.linkElement(&element2);
	Color color(255, 128, 64);
	Group::Item item(
		&group,
		&color,
		Color::Filters::Normal,
		20
	);
	EXPECT_EQ(item.getName(), "TestGroup");
	item.process(50, nullptr);
	EXPECT_EQ(ledValue1, 159);
	EXPECT_EQ(ledValue2, 159);
}

TEST(ItemsTest, GroupItemWithFilter) {
	Color defaultColor(0, 0, 0);
	Group group(
		"TestGroup",
		defaultColor
	);
	uint8_t
		ledValue1 = 100,
		ledValue2 = 100;
	Element element1(
		"Element1",
		&ledValue1,
		defaultColor,
		0,
		100
	);
	Element element2(
		"Element2",
		&ledValue2,
		defaultColor,
		0,
		100
	);
	group.linkElement(&element1);
	group.linkElement(&element2);
	Color color(255, 128, 64);
	Group::Item item(
		&group,
		&color,
		Color::Filters::Combine,
		20
	);
	item.process(50, nullptr);
	EXPECT_EQ(ledValue1, 129);
	EXPECT_EQ(ledValue2, 129);
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
