/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Color.cpp
 * @since     Jun 22, 2018
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

#include "Color.hpp"

using namespace LEDSpicer::Utilities;

unordered_map<string, const Color> Color::colors;
vector<string> Color::names;
vector<const Color*> Color::randomColors;
const Color Color::On(255, 255, 255);
const Color Color::Off(0, 0, 0);

Color::Color(const string& color) {
	set(color);
}

Color::Color(const string& color, const string& formatHex) {
	set(color, formatHex == "hex");
}

bool Color::operator==(const Color& other) const {
	return r == other.r and g == other.g and b == other.b;
}

bool Color::operator!=(const Color& other) const {
	return r != other.r or g != other.g or b != other.b;
}

void Color::setR(uint8_t color) {
	r = color;
}

void Color::setG(uint8_t color) {
	g = color;
}

void Color::setB(uint8_t color) {
	b = color;
}

void Color::set(uint8_t r, uint8_t g, uint8_t b) {
	setR(r);
	setG(g);
	setB(b);
}

void Color::set(uint32_t newColor) {
	setB(newColor & 0xFF);
	newColor>>= 8;
	setG(newColor & 0xFF);
	newColor>>= 8;
	setR(newColor & 0xFF);
}

void Color::set(const string& color) {
	set(colors[color]);
}

void Color::set(const string& color, bool) {

	if (color.size() != 6)
		set(0, 0, 0);
	else
		try {
			set(std::stoul(color, nullptr, 16));
		}
		catch (const std::invalid_argument &e) {
			set(0, 0, 0);
		}
}

void Color::set(const Color& color) {
	set(color.r, color.g, color.b);
}

Color* Color::set(const Color& color, const Filters& filter, uint8_t percent) {

	switch (filter) {
	case Color::Filters::Combine:
		set(this->transition(color, percent));
		break;

	case Color::Filters::Mask:
		set(this->mask(color.getMonochrome()));
		break;

	case Color::Filters::Invert:
		set(color.invert());
		break;

	default:
		set(color);
		break;

	}
	return this;
}

uint8_t Color::getR() const {
	return r;
}

uint8_t Color::getG() const {
	return g;
}

uint8_t Color::getB() const {
	return b;
}

uint32_t Color::getRGB() const {
	uint32_t number = 0;
	number |= r;
	number<<= 8;
	number |= g;
	number<<= 8;
	number |= b;
	return number;
}

Color Color::fade(float percent) const {
	Color color;
	color.set(r * percent / 100.f, g * percent / 100.f, b * percent / 100.f);
	return color;
}

Color Color::transition(const Color& destination, float percent) const {

	if (percent < 1 or destination == *this)
		return *this;

	if (percent > 99)
		return destination;

	return Color(
		transition(r, destination.r, percent),
		transition(g, destination.g, percent),
		transition(b, destination.b, percent)
	);
}

Color Color::difference(const Color& destination) const {
	return Color(
		r - destination.r,
		g - destination.g,
		b - destination.b
	);
}

Color Color::mask(float intensity) const {
	if (intensity > 254)
		return *this;
	if (intensity < 1)
		return Color();
	return Color(
		r * static_cast<float>(intensity) / 255,
		g * static_cast<float>(intensity) / 255,
		b * static_cast<float>(intensity) / 255
	);
}

Color Color::invert() const {
	return Color(
		255 - r,
		255 - g,
		255 - b
	);
}

uint8_t Color::getMonochrome() const {
	return static_cast<uint8_t>(
		0.301 * static_cast<float>(r) +
		0.587 * static_cast<float>(g) +
		0.114 * static_cast<float>(b)
	);
}

uint8_t Color::transition(uint8_t colorA, uint8_t colorB, float percent) {
	return colorA + (colorB - colorA) * percent / 100;
}

void Color::loadColors(const StringUMap& colorsData, const string& format) {
	for (auto& colorData : colorsData) {
		if (colorData.first == Color_Random)
			continue;
		colors.emplace(colorData.first, Color{colorData.second, format});
		names.push_back(colorData.first);
	}
}

void Color::drawColors() {
	int count = 0;
	for (auto& c : colors) {
		cout << std::left << std::setw(15) << std::setfill(' ') << c.first << " = ";
		c.second.drawHex();
		if (++count % 4 == 0)
			cout << endl;
		else
			cout << " | ";
	}
	if (count % 4 != 0)
		cout << endl;
}

void Color::drawColors(const vector<const Color*>& colors) {
	uint8_t count = 0;
	for (auto& c : colors) {
		c->drawColor();
		if (++count < colors.size())
			cout << ", ";
	}
}

void Color::drawHex() const {
	cout << "#" << std::hex << std::uppercase << std::setfill('0')
		 << std::setw(2) << static_cast<int>(r)
		 << std::setw(2) << static_cast<int>(g)
		 << std::setw(2) << static_cast<int>(b)
		 << std::dec;
}

void Color::drawColor() const {
	cout << getName();
}

string Color::filter2str(const Filters filter) {
	switch (filter) {
	case Filters::Normal:  return "Normal";
	case Filters::Combine: return "Combine";
	case Filters::Mask:    return "Mask";
	case Filters::Invert:  return "Invert";
	}
	throw Error("Invalid filter");
}

Color::Filters Color::str2filter(const string& filter) {
	if (filter == "Normal")  return Filters::Normal;
	if (filter == "Combine") return Filters::Combine;
	if (filter == "Mask")    return Filters::Mask;
	if (filter == "Invert")  return Filters::Invert;
	throw Error("Invalid filter type ") << filter;
}

const vector<string>& Color::getNames() {
	return names;
}

string Color::getName() const {
	if (&On == this)  return "On";
	if (&Off == this) return "Off";
	for (auto& color : colors)
		if (color.second == *this) return color.first;
	return "unknown";
}

const Color& Color::getColor(const string& color) {
	if (hasColor(color))       return colors[color];
	if (color == Color_Random) return *randomColors[std::rand() / ((RAND_MAX + 1u) / randomColors.size())];
	if (color == Color_On)     return On;
	if (color == Color_Off)    return Off;
	throw Error("Unknown color ") << color;
}

bool Color::hasColor(const string& color) {
	return colors.exists(color);
}

void Color::setRandomColors(vector<string> colors) {

	if (randomColors.size())
		return;

	if (colors.empty())
		for (auto& c : Color::colors)
			randomColors.push_back(&c.second);
	else
		for (string& c : colors) {
			if (c == Color_Random)
				throw Error(Color_Random " cannot be part of the list of colors because is getting build");
			if (Color::colors.exists(c))
				randomColors.push_back(&Color::colors[c]);
			else
				throw Error("Unknown color ") << c;
		}
	if (randomColors.empty())
		throw Error("All colors failed for random colors");
	randomColors.shrink_to_fit();
}
