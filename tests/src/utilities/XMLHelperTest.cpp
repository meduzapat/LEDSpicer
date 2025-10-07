/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      XMLHelperTest.cpp
 * @since     Aug 24, 2025
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
#include <fstream>
#include <cstdio>

#include "utilities/XMLHelper.hpp"

using namespace LEDSpicer::Utilities;

// Test fixture to create temporary XML files.
class XMLHelperTest : public ::testing::Test {

protected:

	string
		validFileName          = "valid_test.xml",
		invalidVersionFileName = "invalid_version_test.xml",
		invalidTypeFileName    = "invalid_type_test.xml",
		missingFileName        = "missing_test.xml",
		foreignFileName        = "foreign_test.xml",
		malformedFileName      = "malformed_test.xml";

	void SetUp() override {

		// Create a valid XML file.
		std::ofstream validFile(validFileName);
		validFile << "<?xml version=\"1.0\"?>\n"
				  << "<LEDSpicer version=\"1.1\" type=\"test\">\n"
				  << "  <testNode attr1=\"value1\" attr2=\"value2\"/>\n"
				  << "</LEDSpicer>";
		validFile.close();

		// Create an invalid version XML file.
		std::ofstream invalidVersionFile(invalidVersionFileName);
		invalidVersionFile << "<?xml version=\"1.0\"?>\n"
						   << "<LEDSpicer version=\"0.0\" type=\"test\">\n"
						   << "  <testNode attr1=\"value1\"/>\n"
						   << "</LEDSpicer>";
		invalidVersionFile.close();

		// Create an invalid type XML file.
		std::ofstream invalidTypeFile(invalidTypeFileName);
		invalidTypeFile << "<?xml version=\"1.0\"?>\n"
						 << "<LEDSpicer version=\"1.1\" type=\"wrong\">\n"
						 << "  <testNode attr1=\"value1\"/>\n"
						 << "</LEDSpicer>";
		invalidTypeFile.close();

		// Create a foreign XML file.
		std::ofstream foreignFile(foreignFileName);
		foreignFile << "<?xml version=\"1.0\"?>\n"
					<< "<foreignRoot>\n"
					<< "  <testNode attr1=\"value1\" attr2=\"value2\"/>\n"
					<< "</foreignRoot>";
		foreignFile.close();

		// Create a malformed XML file.
		std::ofstream malformedFile(malformedFileName);
		malformedFile << "<?xml version=\"1.0\"?>\n"
					  << "<LEDSpicer version=\"1.1\" type=\"test\">\n"
					  << "  <testNode attr1=\"value1\">\n"  // Missing close tag
					  << "</LEDSpicer>";
		malformedFile.close();
	}

	void TearDown() override {
		std::remove(validFileName.c_str());
		std::remove(invalidVersionFileName.c_str());
		std::remove(invalidTypeFileName.c_str());
		std::remove(missingFileName.c_str());
		std::remove(foreignFileName.c_str());
		std::remove(malformedFileName.c_str());
	}
};

// Test constructor with valid file.
TEST_F(XMLHelperTest, ConstructorValidFile) {
	EXPECT_NO_THROW({
		XMLHelper helper(validFileName, "test");
		tinyxml2::XMLElement* root = helper.getRoot();
		ASSERT_NE(root, nullptr);
		EXPECT_STREQ(root->Name(), "LEDSpicer");
		EXPECT_STREQ(root->Attribute("version"), "1.1");
		EXPECT_STREQ(root->Attribute("type"), "test");
	});
}

// Test constructor with missing file.
TEST_F(XMLHelperTest, ConstructorMissingFile) {
	EXPECT_THROW({
		XMLHelper helper(missingFileName, "test");
	}, Error);
}

// Test constructor with invalid version.
TEST_F(XMLHelperTest, ConstructorInvalidVersion) {
	EXPECT_THROW({
		XMLHelper helper(invalidVersionFileName, "test");
	}, Error);
}

// Test constructor with invalid type.
TEST_F(XMLHelperTest, ConstructorInvalidType) {
	EXPECT_THROW({
		XMLHelper helper(invalidTypeFileName, "test");
	}, Error);
}

// Test constructor with foreign file.
TEST_F(XMLHelperTest, ConstructorForeignFile) {
	EXPECT_NO_THROW({
		XMLHelper helper(foreignFileName, "");
		tinyxml2::XMLElement* root = helper.getRoot();
		ASSERT_NE(root, nullptr);
		EXPECT_STREQ(root->Name(), "foreignRoot");
	});
}

// Test constructor with malformed XML.
TEST_F(XMLHelperTest, ConstructorMalformedFile) {
	EXPECT_THROW({
		XMLHelper helper(malformedFileName, "test");
	}, Error);
}

// Test processNode with existing node.
TEST_F(XMLHelperTest, ProcessNodeExisting) {
	XMLHelper helper(validFileName, "test");
	StringUMap attrs = helper.processNode("testNode");
	EXPECT_EQ(attrs.size(), 2);
	EXPECT_EQ(attrs["attr1"], "value1");
	EXPECT_EQ(attrs["attr2"], "value2");
}

// Test processNode with missing node.
TEST_F(XMLHelperTest, ProcessNodeMissing) {
	XMLHelper helper(validFileName, "test");
	EXPECT_THROW({
		helper.processNode("missingNode");
	}, Error);
}

// Test static processNode.
TEST_F(XMLHelperTest, StaticProcessNode) {
	XMLHelper helper(validFileName, "test");
	tinyxml2::XMLElement* node = helper.getRoot()->FirstChildElement("testNode");
	ASSERT_NE(node, nullptr);
	StringUMap attrs = XMLHelper::processNode(node);
	EXPECT_EQ(attrs.size(), 2);
	EXPECT_EQ(attrs["attr1"], "value1");
	EXPECT_EQ(attrs["attr2"], "value2");
}

// Test getRoot.
TEST_F(XMLHelperTest, GetRoot) {
	XMLHelper helper(validFileName, "test");
	tinyxml2::XMLElement* root = helper.getRoot();
	ASSERT_NE(root, nullptr);
	EXPECT_STREQ(root->Name(), "LEDSpicer");
}
