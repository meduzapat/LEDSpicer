/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file		Color.cpp
 * @since		Jun 22, 2018
 * @author		Patricio A. Rossi (MeduZa)
 * @copyright	Copyright © 2018 Patricio A. Rossi (MeduZa)
 */

#include "Color.hpp"

using namespace LEDSpicer;

umap<string, Color> Color::colors;

Color::Color(const string& color) {
	set(color);
}

Color::Color(const string& color, const string& formatHex) {
	set(color, formatHex == "hex");
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

Color Color::fade(uint8_t percent) {

	Color color;

	if (percent)
		color.set((float)r * 255 / percent, (float)g * 255 / percent, (float)b * 255  / percent);
	else
		color.set(0, 0, 0);

	return color;
}

Color Color::transition(const Color& destination, uint8_t percent) {

	Color rColor(
		transition(r, destination.r, percent),
		transition(g, destination.g, percent),
		transition(b, destination.b, percent)
	);

	return rColor;
}

uint8_t Color::getMonochrome() const {
	return static_cast<uint8_t>(0.301 * (float)r + 0.587 * (float)g + 0.114 * (float)b);
}

uint8_t Color::transition(uint8_t colorA, uint8_t colorB, uint8_t percent) {
	return (colorA + (colorB - colorA) * (1 / percent));
//    return sqrt((1 - percent) * colorA^2 + percent * colorB^2)
}

void Color::loadColors(umap<string, string> colorsData, const string& format) {
	for (auto colorData : colorsData)
		colors[colorData.first] = std::move(Color(colorData.second, format));
}

void Color::drawColors() {
	for (auto c : Color::colors) {
		cout << std::left << std::setfill(' ') << std::setw(15) << c.first << " = ";
		c.second.drawColor();
	}
}

void Color::drawColor() {
	cout <<
		"#" << std::hex <<
		std::uppercase << std::setfill('0') << std::setw(2) << (int)getR() <<
		std::setfill(' ') << std::setw(15) << std::uppercase <<
		std::setfill('0') << std::setw(2) << (int)getG() <<
		std::setfill(' ') << std::setw(15) << std::uppercase <<
		std::setfill('0') << std::setw(2) << (int)getB() << endl;
	cout << std::dec;
}

string Color::filter2str(Filters filter) {
	switch (filter) {
	case Normal:
		return "Normal";
	case Combine:
		return "Combine";
	case Diff:
		return "Diff";
	}
	return "";
}

Color::Filters Color::str2filter(const string& filter) {
	if (filter == "Normal")
		return Normal;
	if (filter == "Combine")
		return Combine;
	if (filter == "Diff")
		return Diff;
	Log::error("Invalid filter type " + filter + " assuming Combine");
	return Combine;
}
