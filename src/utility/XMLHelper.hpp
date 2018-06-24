/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file		XMLHelper.hpp
 * @since		Jun 14, 2018
 * @author		Patricio A. Rossi (MeduZa)
 * @copyright	Copyright © 2018 Patricio A. Rossi (MeduZa)
 */

#ifndef SRC_LEDSPICER_XMLHELPER_HPP_
#define SRC_LEDSPICER_XMLHELPER_HPP_ 1

// To handle unordered map.
#include <unordered_map>
#ifndef umap
	#define umap std::unordered_map
#endif

// To use vectors.
#include <vector>
using std::vector;

// To process XML files.
#include <tinyxml2.h>

#include "Error.hpp"
#include "Log.hpp"

// Conflicting with tinyxml2 Error function
using LEDError = LEDSpicer::Error;

namespace LEDSpicer {

#define DataDocumentVersion "1.0"

/**
 * LEDSpicer::XMLHelper
 *
 * This class wraps over the tinyxml2 class to handle LEDSpicer XML data files.
 */
class XMLHelper : protected tinyxml2::XMLDocument {

public:

	/**
	 * Creates a new XMLHelper object and open the XML file.
	 *
	 * @param fileName
	 * @throws Error if the file is missing, the body is missing or the version is different.
	 */
	XMLHelper(const string& fileName);

	/**
	 * Reads the attributes from a XML node.
	 *
	 * @param parentElement
	 * @param nodeName
	 * @return A map with the key pairs.
	 * @throws Error if an error happen.
	 */
	umap<string, string> processNode(tinyxml2::XMLElement* nodeElement);

	/**
	 * Reads the attributes from a node by its name.
	 *
	 * @param nodeName The name of the node, note that only the first occurrence will be read.
	 * @return A map with the parameters in that node.
	 * @throws Error if node does not exist or an error happen.
	 */
	umap<string, string> processNode(const string& nodeName);

	/**
	 * Checks if the map subject have the attributeList elements.
	 * This function ignores other elements.
	 *
	 * @param attributeList A list of attributes to check.
	 * @param subjects A map to check.
	 * @param node where will check.
	 * @throws Error if an attribute is missing.
	 */
	void checkAttributes(const vector<string>& attributeList, umap<string, string>& subjects, const string& place);

	/**
	 * Returns a pointer to the root node.
	 * @return
	 */
	tinyxml2::XMLElement* getRoot();

protected:

	/// Pointer to the root element.
	tinyxml2::XMLElement* root = nullptr;

};

}

#endif /* SRC_LEDSPICER_XMLHELPER_HPP_ */
