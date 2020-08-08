/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Color.hpp
 * @since     Jun 22, 2018
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2018 - 2020 Patricio A. Rossi (MeduZa)
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

// To handle unordered map.
#include <unordered_map>
#ifndef umap
	#define umap std::unordered_map
#endif

// To handle IO stream.
#include <iostream>
using std::cout;
using std::endl;

// For ints.
#include <cstdint>

#include <iomanip>
#include <string>
using std::string;

#include <vector>
using std::vector;

#include "Error.hpp"

#ifndef COLOR_HPP_
#define COLOR_HPP_ 1

namespace LEDSpicer {

/**
 * LEDSpicer::Color
 */
class Color {

public:

	/**
	 * Normal:  Will overwrite the background.
	 * Combine: Will combine with the background.
	 * Mask:    Will cover the background with a mask.
	 * Invert:  Will Revert the background color.
	 */
	enum class Filters : uint8_t {Normal, Combine, Mask, Invert};

	enum Channels : uint8_t {Red, Green, Blue};

	/**
	 * Creates a new Color class (black color).
	 */
	Color() = default;

	/**
	 * Creates a new Color class from RGB independent values.
	 * @param r
	 * @param g
	 * @param b
	 */
	Color(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b) {}

	/**
	 * Creates a new Color class from a color name from the color list.
	 * @param color
	 */
	Color(const string& color);

	/**
	 * Creates a new color class from a hex or dec color format.
	 * @param color
	 * @param format
	 */
	Color(const string& color, const string& format);

	virtual ~Color() = default;

	/**
	 * Compare same color.
	 * @param other
	 * @return
	 */
	bool operator==(const Color& other) const;

	/**
	 * Compare different Colors.
	 * @param other
	 * @return
	 */
	bool operator!=(const Color& other) const;

	void setR(uint8_t color);
	void setG(uint8_t color);
	void setB(uint8_t color);

	void set(uint8_t r, uint8_t g, uint8_t b);

	void set(uint32_t color);

	/**
	 * Sets the color from an hex(ex FFFFFF) or dec (ex: 255,255,255) string.
	 * TODO: implement dec if needed.
	 * @param color
	 * @param format true for hex
	 */
	void set(const string& color, bool formatHex);
	void set(const string& color);
	void set(const Color& color);

	/**
	 * Applies a color with a filter.
	 * @param color
	 * @param filter
	 * @return self
	 */
	Color* set(const Color& color, const Filters& filter, uint8_t percent = 50);

	uint8_t getR() const;
	uint8_t getG() const;
	uint8_t getB() const;
	uint32_t getRGB() const;

	/**
	 * Calculates the fade for this color.
	 * @param percent 0 to 100, invisible to visible.
	 * @return
	 */
	Color fade(float percent) const;

	/**
	 * Calculates the transition between this color and a destination color.
	 * @param destination
	 * @param percent
	 * @return
	 */
	Color transition(const Color& destination, float percent) const;

	/**
	 * Subtracts a color from another color.
	 * @param destination
	 * @return
	 */
	Color difference(const Color& destination) const;

	/**
	 * Creates a mask over the color, more intense the mask, more visible the color.
	 * @param intensity 0 not visible 255 full visible
	 * @return
	 */
	Color mask(float intensity) const;

	/**
	 * @return Returns the color inverted.
	 */
	Color invert() const;

	/**
	 * Returns the monochrome version of the color.
	 * @return
	 */
	uint8_t getMonochrome() const;

	/**
	 * Creates a linear transition between two colors tones.
	 * @param colorA
	 * @param colorB
	 * @param percent
	 * @return
	 */
	static uint8_t transition(uint8_t colorA, uint8_t colorB, float percent);

	/**
	 * Imports a set of colors to be used by the program.
	 * @param colorsData
	 * @param format (not used yet, only hex) hex or dec
	 */
	static void loadColors(const umap<string, string>& colorsData, const string& format);

	/**
	 * Output the internal list of colors.
	 */
	static void drawColors();

	/**
	 * Output the colors from an array of colors.
	 * @param colors
	 */
	static void drawColors(vector<const Color*>& colors);

	/**
	 * Output the current color HEX.
	 */
	void drawHex() const;

	/**
	 * Draws the current color name.
	 */
	void drawColor() const;

	/**
	 * Converts the filter code into string.
	 * @param filter
	 * @return
	 */
	static string filter2str(Filters filter);

	/**
	 * Converts a string into a filter.
	 * @param filter
	 * @return
	 */
	static Filters str2filter(const string& filter);

	/**
	 * Returns a color by name.
	 * @param color
	 * @return
	 */
	static const Color& getColor(const string& color);

	/**
	 * Returns a pointer to an array of color names.
	 * @return
	 */
	static const vector<string>& getNames();

	/**
	 * Returns the color name.
	 * @return
	 */
	string getName() const;

	/**
	 * Specifies the random colors to use.
	 * NOTE: This function may be allocates a new vector that is never deleted (but not a memory leak).
	 * @param colors
	 */
	static void setRandomColors(vector<string> colors);

protected:

	/// Loaded colors.
	static umap<string, Color> colors;

	/// Possible values for random.
	static vector<Color*> randomColors;

	/// List of color names.
	static vector<string> names;

	uint8_t
		r = 0,
		g = 0,
		b = 0;
};

} /* namespace LEDSpicer */

#endif /* COLOR_HPP_ */
