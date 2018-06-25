/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file		XMLHelper.cpp
 * @since		Jun 14, 2018
 * @author		Patricio A. Rossi (MeduZa)
 * @copyright	Copyright © 2018 Patricio A. Rossi (MeduZa)
 */
#include "XMLHelper.hpp"

using namespace LEDSpicer;

XMLHelper::XMLHelper(const string& fileName, const string& fileType) {

	Log::debug("Reading " + fileName);

	if (LoadFile(fileName.c_str()) != tinyxml2::XML_SUCCESS)
		throw LEDError("Unable to read the file " + fileName);

	root = RootElement();
	if (not root or std::strcmp(root->Name(), PACKAGE_NAME))
		throw LEDError("Unknown data file");

	if (root->Attribute("Version") and not std::strcmp(root->Attribute("Version"), DataDocumentVersion))
		throw LEDError("Invalid data file version, needed " DataDocumentVersion);

	if (root->Attribute("Type") and fileType == root->Attribute("Version"))
		throw LEDError("Invalid data file type, needed " + fileType);
}

umap<string, string> XMLHelper::processNode(tinyxml2::XMLElement* nodeElement) {

	umap<string, string> groupValues;

	const tinyxml2::XMLAttribute* pAttrib = nodeElement->FirstAttribute();

	while (pAttrib) {
		string value = pAttrib->Value();
		groupValues.emplace(pAttrib->Name(), value);
		pAttrib = pAttrib->Next();
	}

	return std::move(groupValues);
}

umap<string, string> XMLHelper::processNode(const string& nodeElement) {

	tinyxml2::XMLElement* node = root->FirstChildElement(nodeElement.c_str());
	if (not node)
		throw LEDError("Missing " + nodeElement + " section.");

	return std::move(processNode(node));
}

tinyxml2::XMLElement* XMLHelper::getRoot() {
	return root;
}
