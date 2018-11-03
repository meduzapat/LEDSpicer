/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Utility.hpp
 * @since     Jun 24, 2018
 * @author    Patricio A. Rossi (MeduZa)
 * @copyright Copyright © 2018 Patricio A. Rossi (MeduZa)
 */

// To handle unordered map.
#include <unordered_map>
#ifndef umap
	#define umap std::unordered_map
#endif

#include <vector>
using std::vector;

// to handle special formats.
#include <sstream>

#include "Error.hpp"

#ifndef UTILITY_HPP_
#define UTILITY_HPP_ 1

namespace LEDSpicer {

class Utility {

public:

	static void ltrim(string& text);

	static void rtrim(string& text);

	static void trim(string& text);

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

	/**
	 * Explodes a string into chunks using a delimiter.
	 *
	 * @param text
	 * @param delimiter
	 * @param limit
	 * @return an array with the string chunks.
	 */
	static vector<string> explode(
		const string& text,
		const char delimiter,
		const size_t limit = 0
	);

	/**
	 * Verify that a number is between two vales
	 * @param val subject
	 * @param min minimum
	 * @param max maximum
	 * @param truncate
	 *        if set to true, the value will be changed to min or max depending on what it fails,
	 *        this mode always returns true.
	 * @return
	 */
	static bool verifyValue(int& val, int min, int max, bool truncate = true);

	/**
	 * Same but for smaller numbers.
	 * @param val
	 * @param min
	 * @param max
	 * @param truncate
	 * @return
	 */
	static bool verifyValue(uint8_t& val, uint8_t min, uint8_t max, bool truncate = true);

	/**
	 * converts an integer into is hexadecimal representation.
	 * @param number
	 * @return
	 */
	static string hex2str(int number);
};

} // namespace

#endif /* UTILITY_HPP_ */
