/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      UtilityTest.cpp
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
#include "utilities/Utility.hpp"

using namespace LEDSpicer::Utilities;

class UtilityTest : public ::testing::Test {

protected:

	void SetUp() override {
		// Reset global flags for each test.
		Utility::globalFlags = 0;
	}
};

TEST_F(UtilityTest, GlobalFlags) {
	EXPECT_EQ(Utility::globalFlags, 0);
	Utility::globalFlags = 42;
	EXPECT_EQ(Utility::globalFlags, 42);
}

TEST_F(UtilityTest, ParseNumber) {
	// Valid numbers.
	EXPECT_EQ(Utility::parseNumber("123", "Invalid"), 123);
	EXPECT_EQ(Utility::parseNumber("-456", "Invalid"), -456);
	EXPECT_EQ(Utility::parseNumber("0", "Invalid"), 0);
	EXPECT_EQ(Utility::parseNumber("12.3", "Invalid"), 12);
	EXPECT_EQ(Utility::parseNumber("1abc2", "Invalid"), 1);

	// Invalid format.
	EXPECT_THROW(Utility::parseNumber("abc", "Parse error"), Error);
	EXPECT_THROW(Utility::parseNumber("abc2", "Parse error"), Error);

	// Out of range.
	EXPECT_THROW(Utility::parseNumber("2147483648", "Parse error"), Error); // INT_MAX + 1
	EXPECT_THROW(Utility::parseNumber("-2147483649", "Parse error"), Error); // INT_MIN - 1

	// Empty string.
	EXPECT_THROW(Utility::parseNumber("", "Parse error"), Error);
}

TEST_F(UtilityTest, Ltrim) {
	string text1 = "   hello";
	Utility::ltrim(text1);
	EXPECT_EQ(text1, "hello");

	string text2 = "\t\n\rhello";
	Utility::ltrim(text2);
	EXPECT_EQ(text2, "hello");

	string text3 = "hello  ";
	Utility::ltrim(text3);
	EXPECT_EQ(text3, "hello  ");

	string text4 = "   ";
	Utility::ltrim(text4);
	EXPECT_EQ(text4, "");

	string text5 = "";
	Utility::ltrim(text5);
	EXPECT_EQ(text5, "");
}

TEST_F(UtilityTest, Rtrim) {
	string text1 = "hello   ";
	Utility::rtrim(text1);
	EXPECT_EQ(text1, "hello");

	string text2 = "hello\t\n\r";
	Utility::rtrim(text2);
	EXPECT_EQ(text2, "hello");

	string text3 = "  hello";
	Utility::rtrim(text3);
	EXPECT_EQ(text3, "  hello");

	string text4 = "   ";
	Utility::rtrim(text4);
	EXPECT_EQ(text4, "");

	string text5 = "";
	Utility::rtrim(text5);
	EXPECT_EQ(text5, "");
}

TEST_F(UtilityTest, Trim) {
	string text1 = "   hello   ";
	Utility::trim(text1);
	EXPECT_EQ(text1, "hello");

	string text2 = "\t\n\rhello\t\n\r";
	Utility::trim(text2);
	EXPECT_EQ(text2, "hello");

	string text3 = "   ";
	Utility::trim(text3);
	EXPECT_EQ(text3, "");

	string text4 = "";
	Utility::trim(text4);
	EXPECT_EQ(text4, "");
}

TEST_F(UtilityTest, CheckAttributes) {
	StringUMap subjects = {
		{"key1", "value1"},
		{"key2", "value2"},
		{"key3", "value3"}
	};

	vector<string> attrList1 = {"key1", "key2"};
	EXPECT_NO_THROW(Utility::checkAttributes(attrList1, subjects, "test"));

	vector<string> attrList2 = {"key1", "missing"};
	EXPECT_THROW(Utility::checkAttributes(attrList2, subjects, "test"), Error);

	vector<string> emptyList;
	EXPECT_NO_THROW(Utility::checkAttributes(emptyList, subjects, "test"));

	StringUMap emptySubjects;
	EXPECT_THROW(Utility::checkAttributes(attrList1, emptySubjects, "test"), Error);
}

TEST_F(UtilityTest, Explode) {
	// Basic explode.
	vector<string> result1 = Utility::explode("a,b,c,d", ',');
	EXPECT_EQ(result1, vector<string>({"a", "b", "c", "d"}));

	// With limit.
	vector<string> result2 = Utility::explode("a,b,c,d", ',', 2);
	EXPECT_EQ(result2, vector<string>({"a", "b"}));

	// No delimiter.
	vector<string> result3 = Utility::explode("hello", ',');
	EXPECT_EQ(result3, vector<string>({"hello"}));

	// Empty string.
	vector<string> result4 = Utility::explode("", ',');
	EXPECT_TRUE(result4.empty());

	// Trailing delimiter.
	vector<string> result5 = Utility::explode("a,b,", ',');
	EXPECT_EQ(result5, vector<string>({"a", "b", ""}));

	// Leading delimiter.
	vector<string> result6 = Utility::explode(",a,b", ',');
	EXPECT_EQ(result6, vector<string>({"", "a", "b"}));

	// Multiple delimiters.
	vector<string> result7 = Utility::explode("a,,b", ',');
	EXPECT_EQ(result7, vector<string>({"a", "", "b"}));
}

TEST_F(UtilityTest, Implode) {
	// Empty vector.
	vector<string> empty;
	EXPECT_EQ(Utility::implode(empty, ','), "");

	// Single element.
	vector<string> single = {"hello"};
	EXPECT_EQ(Utility::implode(single, ','), "hello");

	// Multiple elements.
	vector<string> multiple = {"a", "b", "c"};
	EXPECT_EQ(Utility::implode(multiple, ','), "a,b,c");

	// Different delimiter.
	EXPECT_EQ(Utility::implode(multiple, '|'), "a|b|c");

	// Empty strings.
	vector<string> empties = {"", "", ""};
	EXPECT_EQ(Utility::implode(empties, ','), ",,");

	// Mixed.
	vector<string> mixed = {"", "x", ""};
	EXPECT_EQ(Utility::implode(mixed, '-'), "-x-");
}

TEST_F(UtilityTest, VerifyValue) {
	// Int: in range.
	int val1 = 5;
	EXPECT_TRUE(Utility::verifyValue(val1, 0, 10));
	EXPECT_EQ(val1, 5);

	// Int: below, truncate.
	int val2 = -1;
	EXPECT_TRUE(Utility::verifyValue(val2, 0, 10));
	EXPECT_EQ(val2, 0);

	// Int: above, truncate.
	int val3 = 15;
	EXPECT_TRUE(Utility::verifyValue(val3, 0, 10));
	EXPECT_EQ(val3, 10);

	// Int: below, no truncate.
	int val4 = -1;
	EXPECT_FALSE(Utility::verifyValue(val4, 0, 10, false));
	EXPECT_EQ(val4, -1);

	// Int: above, no truncate.
	int val5 = 15;
	EXPECT_FALSE(Utility::verifyValue(val5, 0, 10, false));
	EXPECT_EQ(val5, 15);

	// Float: in range.
	float fval1 = 5.5f;
	EXPECT_TRUE(Utility::verifyValue(fval1, 0.0f, 10.0f));
	EXPECT_FLOAT_EQ(fval1, 5.5f);

	// Float: below, truncate.
	float fval2 = -1.5f;
	EXPECT_TRUE(Utility::verifyValue(fval2, 0.0f, 10.0f));
	EXPECT_FLOAT_EQ(fval2, 0.0f);

	// Float: above, truncate.
	float fval3 = 15.5f;
	EXPECT_TRUE(Utility::verifyValue(fval3, 0.0f, 10.0f));
	EXPECT_FLOAT_EQ(fval3, 10.0f);

	// Min == max.
	int val6 = 10;
	EXPECT_TRUE(Utility::verifyValue(val6, 10, 10));
	EXPECT_EQ(val6, 10);

	int val7 = 9;
	EXPECT_TRUE(Utility::verifyValue(val7, 10, 10));
	EXPECT_EQ(val7, 10);
}

TEST_F(UtilityTest, Hex2str) {
	EXPECT_EQ(Utility::hex2str(0), "0x0");
	EXPECT_EQ(Utility::hex2str(255), "0xff");
	EXPECT_EQ(Utility::hex2str(4096), "0x1000");
	EXPECT_EQ(Utility::hex2str(-1), "0xffffffff");
	EXPECT_EQ(Utility::hex2str(0x1234), "0x1234");
	EXPECT_EQ(Utility::hex2str(0xabcdef), "0xabcdef");
}

TEST_F(UtilityTest, RemoveChar) {
	EXPECT_EQ(Utility::removeChar("hello, world,", ','), "hello world");
	EXPECT_EQ(Utility::removeChar("   spaced   ", ' '), "spaced");
	EXPECT_EQ(Utility::removeChar("", 'a'), "");
	EXPECT_EQ(Utility::removeChar("aaa", 'a'), "");
	EXPECT_EQ(Utility::removeChar("abc", 'd'), "abc");
}

TEST_F(UtilityTest, ExtractChars) {
	EXPECT_EQ(Utility::extractChars("a1b2c3", '0', '9'), "123");
	EXPECT_EQ(Utility::extractChars("hello123world", 'a', 'z'), "helloworld");
	EXPECT_EQ(Utility::extractChars("", 'a', 'z'), "");
	EXPECT_EQ(Utility::extractChars("123", 'a', 'z'), "");
	EXPECT_EQ(Utility::extractChars("aBcDeF", 'A', 'Z'), "BDF"); // Case sensitive.
}

TEST_F(UtilityTest, GetHomeDir) {
	string home = Utility::getHomeDir();
	EXPECT_FALSE(home.empty());
	EXPECT_NE(home.back(), '/');
	EXPECT_EQ(home[0], '/');
	// If HOME unset, "/root".
}

TEST_F(UtilityTest, GetConfigDir) {
	string config = Utility::getConfigDir();
	EXPECT_FALSE(config.empty());
	EXPECT_TRUE(config.find("/.local/share/" PROJECT_NAME) != string::npos);
	EXPECT_EQ(config[0], '/');
}

// Main function for running tests
int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
