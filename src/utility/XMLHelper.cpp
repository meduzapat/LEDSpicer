/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      XMLHelper.cpp
 * @since     Jun 14, 2018
 * @author    Patricio A. Rossi (MeduZa)
 * @copyright Copyright © 2018 Patricio A. Rossi (MeduZa)
 */
#include "XMLHelper.hpp"

using namespace LEDSpicer;

XMLHelper::XMLHelper(const string& fileName, const string& fileType) {

	LogInfo("Reading " + fileName);

	if (LoadFile(fileName.c_str()) != tinyxml2::XML_SUCCESS)
		throw LEDError("Unable to read the file " + fileName);

	root = RootElement();
	if (not root or std::strcmp(root->Name(), PACKAGE_NAME))
		throw LEDError("Unknown or invalid data file");

	if (not root->Attribute("version") or std::strcmp(root->Attribute("version"), DATA_VERSION))
		throw LEDError("Invalid data file version, needed " DATA_VERSION);

	if (not root->Attribute("type") or fileType != root->Attribute("type"))
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

	return groupValues;
}

umap<string, string> XMLHelper::processNode(const string& nodeElement) {

	tinyxml2::XMLElement* node = root->FirstChildElement(nodeElement.c_str());
	if (not node)
		throw LEDError("Missing " + nodeElement + " section.");

	return processNode(node);
}

tinyxml2::XMLElement* XMLHelper::getRoot() {
	return root;
}
