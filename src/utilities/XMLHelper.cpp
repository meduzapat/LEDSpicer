/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      XMLHelper.cpp
 * @since     Jun 14, 2018
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

#include "XMLHelper.hpp"

// Alias to avoid name conflict with tinyxml2::XMLDocument::Error()
using LEDError = LEDSpicer::Utilities::Error;

using namespace LEDSpicer::Utilities;

XMLHelper::XMLHelper(const string& fileName, const string& fileType) {

	LogInfo("Reading " + fileName);

	if (LoadFile(fileName.c_str()) != tinyxml2::XML_SUCCESS)
		throw LEDError("Unable to read the file ") << fileName << " " <<
			ErrorStr() << " at line " << ErrorLineNum();
	root = RootElement();
	if (fileType != XML_FILE_FOREIGN) {
		if (not root or std::strcmp(root->Name(), PROJECT_NAME))
			throw LEDError("Unknown or invalid data file");

		if (not root->Attribute("version") or std::strcmp(root->Attribute("version"), DATA_VERSION))
			throw LEDError("Invalid data file version, needed " DATA_VERSION);

		if (not root->Attribute("type") or fileType != root->Attribute("type"))
			throw LEDError("Invalid data file type, needed ") << fileType;
	}
}

StringUMap XMLHelper::processNode(tinyxml2::XMLElement* const nodeElement) {

	StringUMap groupValues;

	const tinyxml2::XMLAttribute* pAttrib = nodeElement->FirstAttribute();

	while (pAttrib) {
		string value = pAttrib->Value();
		groupValues.emplace(pAttrib->Name(), value);
		pAttrib = pAttrib->Next();
	}

	return groupValues;
}

StringUMap XMLHelper::processNode(const string& nodeElement) const {

	tinyxml2::XMLElement* node = root->FirstChildElement(nodeElement.c_str());
	if (not node)
		throw LEDError("Missing ") << nodeElement << " section.";

	return processNode(node);
}

tinyxml2::XMLElement* XMLHelper::getRoot() const {
	return root;
}
