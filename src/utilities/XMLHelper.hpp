/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      XMLHelper.hpp
 * @since     Jun 14, 2018
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

// To process XML files.
#include <tinyxml2.h>

#include "Utility.hpp"
#include "Log.hpp"

#pragma once

// No better place that makes sense for config file
#define CONFIG_FILE  PROJECT_CONF_DIR "/ledspicer.conf"
#define XML_FILE_FOREIGN ""

namespace LEDSpicer::Utilities {

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
	XMLHelper(const string& fileName, const string& fileType);

	/**
	 * Reads the attributes from a XML node.
	 *
	 * @param parentElement
	 * @param nodeName
	 * @return A map with the key pairs.
	 * @throws Error if an error happen.
	 */
	static StringUMap processNode(tinyxml2::XMLElement* const nodeElement);

	/**
	 * Reads the attributes from a node by its name on the root.
	 *
	 * @param nodeName The name of the node, note that only the first occurrence will be read.
	 * @return A map with the parameters in that node.
	 * @throws Error if node does not exist or an error happen.
	 */
	StringUMap processNode(const string& nodeName) const;

	/**
	 * Returns a pointer to the root node.
	 * @return
	 */
	tinyxml2::XMLElement* getRoot() const;

protected:

	/// Pointer to the root element.
	tinyxml2::XMLElement* root = nullptr;

};

} // namespace
