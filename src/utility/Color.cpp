/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Color.cpp
 * @since     Jun 22, 2018
 * @author    Patricio A. Rossi (MeduZa)
 * @copyright Copyright © 2018 - 2019 Patricio A. Rossi (MeduZa)
 */

#include "Color.hpp"

using namespace LEDSpicer;

umap<string, Color> Color::colors;
vector<string> Color::names;

Color::Color(const string& color) {
	set(color);
}

Color::Color(const string& color, const string& formatHex) {
	set(color, formatHex == "hex");
}

bool Color::operator==(const Color& other) {
	return r == other.r and g == other.g and b == other.b;
}

bool Color::operator!=(const Color& other) {
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

void Color::set(const string& color, bool format) {

	if (color.empty() or color.size() != 6)
		set(0, 0, 0);
	else
		set(std::stoul(color, nullptr, 16));
}

void Color::set(const Color& color) {
	set(color.r, color.g, color.b);
}

Color* Color::set(const Color& color, const Filters& filter, uint8_t percent) {

	switch (filter) {
	case Color::Filters::Normal:
		set(color);
		break;

	case Color::Filters::Combine:
		set(
			this->transition(color, percent)
		);
		break;

	case Color::Filters::Mask:
		set(
			this->mask(color.getMonochrome())
		);
		break;

	case Color::Filters::Invert:
		set(
			this->invert()
		);
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
	number | g;
	number<<= 8;
	number | b;
	return number;
}

Color Color::fade(float percent) const {
	Color color;
	color.set(r * percent / 100, g * percent / 100, b * percent / 100);
	return color;
}

Color Color::transition(const Color& destination, float percent) const {
	if (percent < 1)
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

void Color::loadColors(const umap<string, string>& colorsData, const string& format) {
	for (auto& colorData : colorsData) {
		colors[colorData.first] = std::move(Color(colorData.second, format));
		names.push_back(colorData.first);
	}
}

void Color::drawColors() {
	for (auto& c : colors) {
		cout << std::left << std::setfill(' ') << std::setw(15) << c.first << " = ";
		c.second.drawHex();
		cout << endl;
	}
}

void Color::drawColors(vector<const Color*>& colors) {
	uint8_t count = 0;
	for (auto c : colors) {
		c->drawColor();
		if (++count < colors.size())
			cout << ", ";
	}
}

void Color::drawHex() const {
	cout << "#" << std::hex << std::uppercase <<
		std::setfill('0') << std::setw(2)  << (int)getR()    <<
		std::setfill(' ') << std::setw(15) << std::uppercase <<
		std::setfill('0') << std::setw(2)  << (int)getG()    <<
		std::setfill(' ') << std::setw(15) << std::uppercase <<
		std::setfill('0') << std::setw(2)  << (int)getB();
	cout << std::dec;
}

void Color::drawColor() const {
	cout << getName();
}

string Color::filter2str(Filters filter) {
	switch (filter) {
	case Filters::Normal:
		return "Normal";
	case Filters::Combine:
		return "Combine";
	case Filters::Mask:
		return "Mask";
	case Filters::Invert:
		return "Invert";
	}
	throw Error("Invalid filter");
}

Color::Filters Color::str2filter(const string& filter) {
	if (filter == "Normal")
		return Filters::Normal;
	if (filter == "Combine")
		return Filters::Combine;
	if (filter == "Mask")
		return Filters::Mask;
	if (filter == "Invert")
		return Filters::Invert;
	throw Error("Invalid filter type " + filter);
}

const vector<string>& Color::getNames() {
	return names;
}

string Color::getName() const {
	for (auto& color : colors)
		if (color.second == *this)
			return color.first;
	return "unknown";
}

const Color& Color::getColor(const string& color) {
	if (colors.count(color))
		return colors[color];
	throw Error("Unknown color " + color);
}
