/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      MessageTest.cpp
 * @since     Aug 21, 2025
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
#include "utilities/Message.hpp"

using namespace LEDSpicer::Utilities;

class MessageTest : public ::testing::Test {

protected:
	void SetUp() override {
		// Reset any global state if needed
	}
};

TEST_F(MessageTest, Constructors) {
	// Default constructor
	Message m1;
	EXPECT_EQ(m1.getType(), Message::Types::Invalid);
	EXPECT_EQ(m1.getFlags(), 0);
	EXPECT_TRUE(m1.getData().empty());

	// Type constructor
	Message m2(Message::Types::LoadProfile);
	EXPECT_EQ(m2.getType(), Message::Types::LoadProfile);
	EXPECT_TRUE(m2.getData().empty());
}

TEST_F(MessageTest, SettersAndGetters) {
	Message m;

	// Set type
	m.setType(Message::Types::SetElement);
	EXPECT_EQ(m.getType(), Message::Types::SetElement);

	// Set flags
	m.setFlags(FLAG_NO_ANIMATIONS | FLAG_FORCE_RELOAD);
	EXPECT_EQ(m.getFlags(), FLAG_NO_ANIMATIONS | FLAG_FORCE_RELOAD);

	// Set data
	vector<string> data = {"elem1", "colorRed"};
	m.setData(data);
	EXPECT_EQ(m.getData(), data);

	// Add data
	m.addData("extra");
	EXPECT_EQ(m.getData().size(), 3);
	EXPECT_EQ(m.getData()[2], "extra");

	// Reset
	m.reset();
	EXPECT_EQ(m.getType(), Message::Types::Invalid);
	EXPECT_TRUE(m.getData().empty());
}

TEST_F(MessageTest, ToString) {
	Message m(Message::Types::SetElement);
	m.setFlags(FLAG_NO_ANIMATIONS);
	vector<string> data = {"elem1", "colorRed"};
	m.setData(data);

	string expected =
		"elem1" + string(1, RECORD_SEPARATOR) +
		"colorRed" + string(1, RECORD_SEPARATOR) +
		std::to_string(FLAG_NO_ANIMATIONS) + string(1, RECORD_SEPARATOR) +
		std::to_string(static_cast<uint8_t>(Message::Types::SetElement)) + string(1, RECORD_SEPARATOR);

	EXPECT_EQ(m.toString(), expected);

	// Empty data
	Message m2(Message::Types::Invalid);
	EXPECT_EQ(m2.toString(), "0" + string(1, RECORD_SEPARATOR) + "0" + string(1, RECORD_SEPARATOR));
}

TEST_F(MessageTest, ToHumanString) {
	Message m;
	vector<string> data = {"group1" + string(1, FIELD_SEPARATOR) + "elem1", "colorRed"};
	m.setData(data);

	string human = m.toHumanString();
	EXPECT_EQ(human, "group1,elem1|colorRed");

	// With group separator
	data = {"groupA" + string(1, GROUP_SEPARATOR) + "elemX", "colorBlue"};
	m.setData(data);
	human = m.toHumanString();
	EXPECT_EQ(human, "groupA:elemX|colorBlue");

	// Empty data
	Message m2;
	EXPECT_EQ(m2.toHumanString(), "");
}

TEST_F(MessageTest, TypeConversions) {
	// type2str
	EXPECT_EQ(Message::type2str(Message::Types::LoadProfile), "LoadProfile");
	EXPECT_EQ(Message::type2str(Message::Types::FinishAllProfiles), "FinishAllProfiles");
	EXPECT_EQ(Message::type2str(Message::Types::CraftProfile), "CraftProfile");

	// Invalid type
	EXPECT_THROW(Message::type2str(static_cast<Message::Types>(255)), Error);

	// str2type
	EXPECT_EQ(Message::str2type("SetGroup"), Message::Types::SetGroup);
	EXPECT_EQ(Message::str2type("ClearAllElements"), Message::Types::ClearAllElements);

	// Invalid string
	EXPECT_THROW(Message::str2type("InvalidType"), Error);
	EXPECT_THROW(Message::str2type(""), Error);
}

TEST_F(MessageTest, FlagConversions) {
	// flag2str
	uint8_t flags = FLAG_NO_ANIMATIONS | FLAG_NO_INPUTS | FLAG_FORCE_RELOAD;
	string flagStr = Message::flag2str(flags);
	EXPECT_EQ(flagStr, "NO_ANIMATIONS NO_INPUTS FORCE_RELOAD");  // Order as in code

	// No flags
	EXPECT_EQ(Message::flag2str(0), "");

	// str2flag
	uint8_t currentFlags = 0;

	// Invalid flag ignored (no throw, as per code)
	currentFlags = 0;
	Message::str2flag(currentFlags, "INVALID_FLAG");
	EXPECT_EQ(currentFlags, 0);

	// Empty string
	currentFlags = 1;
	Message::str2flag(currentFlags, "");
	EXPECT_EQ(currentFlags, 1);  // Unchanged
}

// Main function for running tests
int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
