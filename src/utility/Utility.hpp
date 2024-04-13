/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Utility.hpp
 * @since     Jun 24, 2018
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2018 - 2024 Patricio A. Rossi (MeduZa)
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

#include <cstdint>
#include <algorithm>
// To handle unordered map.
#include <unordered_map>
#ifndef umap
	#define umap std::unordered_map
#endif

#include <vector>
using std::vector;

// to handle special formats.
#include <sstream>
#include <unistd.h> // for setenv

#include "Error.hpp"

#ifndef UTILITY_HPP_
#define UTILITY_HPP_ 1

#define PERCENT(x , y) ((x) * 100 / (y))

#define SEPARATOR "------------------------------"
#define GROUP_SEPARATOR static_cast<char>(29)
#define FIELD_SEPARATOR  static_cast<char>(30)
#define RECORD_SEPARATOR static_cast<char>(31)
#define ID_SEPARATOR       ','
#define ID_GROUP_SEPARATOR '|'
#define WAYS_INDICATOR     "WAYS"

/**
 * Flags
 * @{
 */
/// Flag indicating the absence of animations.
#define FLAG_NO_ANIMATIONS        1
/// Flag indicating the absence of inputs.
#define FLAG_NO_INPUTS            2
/// Flag indicating the absence of start transitions.
#define FLAG_NO_START_TRANSITIONS 4
/// Flag indicating the absence of end transitions.
#define FLAG_NO_END_TRANSITIONS   8
/// Flag indicating the absence of a rotator.
#define FLAG_NO_ROTATOR           16
/// Flag indicating a force reload.
#define FLAG_FORCE_RELOAD         32
/// Flag indicating a replacement.
#define FLAG_REPLACE              64
/// @}

namespace LEDSpicer {

class Utility {

public:

	/// Current global flags.
	static uint8_t globalFlags;

	/**
	 * Attempts to extract a number from a string.
	 * @param number A string representing a number.
	 * @param errorMessage The message to display in case of error.
	 * @throws Error if the string is not valid.
	 * @return The extracted number.
	 */
	static int parseNumber(const string& number, const string& errorMessage);

	/**
	 * Removes spaces from the left
	 * @param text
	 */
	static void ltrim(string& text);

	/**
	 * Removes spaces from the right.
	 * @param text
	 */
	static void rtrim(string& text);

	/**
	 * Removes spaces from both sides.
	 * @param text
	 */
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
	 * Merge an array into a string using a delimiter.
	 * @param values
	 * @param delimiter
	 * @return
	 */
	static string implode(const vector<string>& values, const char& delimiter);

	/**
	 * Verify that a number is between two vales
	 * @param val subject
	 * @param min minimum
	 * @param max maximum
	 * @param truncate
	 *        if set to true, the value will be changed to min or max depending on what it fails,
	 *        this mode always returns true.
	 * @return true if the value is acceptable.
	 */
	template <typename T>
	static bool verifyValue(T& val, T min, T max, bool truncate = true) {
		if (val < min) {
			if (truncate) {
				val = min;
				return true;
			}
			return false;
		}
		if (val > max) {
			if (truncate) {
				val = max;
				return true;
			}
			return false;
		}
		return true;
	}

	/**
	 * Converts an integer into is hexadecimal representation.
	 * @param number
	 * @return
	 */
	static string hex2str(int number);

	/**
	 * Removes characters from string.
	 * @param str
	 * @param c
	 * @return
	 */
	static string removeChar(const string& str, char c);

	/**
	 * Extracts characters from a string.
	 * @param str
	 * @param from
	 * @param to
	 * @return
	 */
	static string extractChars(const string& str, char from, char to);

	/**
	 * @return the current user home without the ending /.
	 */
	static const string getHomeDir();

	/**
	 * @return the current user program config dir without the ending /.
	 */
	static const string getConfigDir();
};

} // namespace

#endif /* UTILITY_HPP_ */
