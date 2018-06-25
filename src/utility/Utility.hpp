/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file		Utility.hpp
 * @since		Jun 24, 2018
 * @author		Patricio A. Rossi (MeduZa)
 * @copyright	Copyright © 2018 Patricio A. Rossi (MeduZa)
 */

// To handle unordered map.
#include <unordered_map>
#ifndef umap
	#define umap std::unordered_map
#endif

#include <vector>
using std::vector;

#include "Error.hpp"

#ifndef UTILITY_HPP_
#define UTILITY_HPP_ 1

namespace LEDSpicer {

class Utility {

public:

	/**
	 * Checks if the map subject have the attributeList elements.
	 * This function ignores other elements.
	 *
	 * @param attributeList A list of attributes to check.
	 * @param subjects A map to check.
	 * @param node where will check.
	 * @throws Error if an attribute is missing.
	 */
	static void checkAttributes(
		const vector<string>& attributeList,
		const umap<string, string>& subjects,
		const string& place
	);
};

} // namespace

#endif /* UTILITY_HPP_ */
