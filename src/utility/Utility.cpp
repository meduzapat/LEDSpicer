/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Utility.cpp
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

#include "Utility.hpp"

using namespace LEDSpicer;

uint8_t Utility::globalFlags = 0;

int Utility::parseNumber(const string& number, const string& errorMessage) {
	try {
		return std::stoi(number);
	} catch (...) {
		if (errorMessage.size())
			throw Error(errorMessage);
	}
	return 0;
}

void Utility::ltrim(string& text) {
	size_t chars = 0;
	for (size_t c = 0; c < text.size(); c++) {
		if (text[c] > 32)
			break;
		chars ++;
	}
	if (chars)
		text.erase(0, chars);
}

void Utility::rtrim(string& text) {
	size_t chars = 0;
	for (size_t c = text.size(); c > 0; c--) {
		if (text[c] > 32)
			break;
		chars ++;
	}
	if (chars)
		text.resize(text.size() - chars + 1);
}

void Utility::trim(string& text) {
	ltrim(text);
	rtrim(text);
}

void Utility::checkAttributes(
	const vector<string>& attributeList,
	const umap<string, string>& subjects,
	const string& place)
{
	for (string attribute : attributeList)
		if (not subjects.count(attribute))
			throw Error("Missing attribute '" + attribute + "' inside " + place);
}

vector<string> Utility::explode(const string& text, const char delimiter, const size_t limit) {

	vector<string> temp;

	if (text.empty())
		return temp;

	size_t start = 0, end = 0;

	for (size_t c = 0; limit ? c < limit : true; c++) {
		end = text.find(delimiter, start);
		if (end == string::npos) {
			temp.push_back(text.substr(start));
			break;
		}
		temp.push_back(text.substr(start, end - start));
		start = end + 1;
	}

	return temp;
}

string Utility::implode(const vector<string>& values, const char& delimiter) {
	string r;
	if (values.empty())
		return r;
	for (const string& s : values) {
		r += s + delimiter;
	}
	r.resize(r.size() - 1);
	return r;
}

string Utility::hex2str(int number) {
	std::stringstream stream;
	stream << std::hex << std::showbase << number;
	return string(stream.str());
}

string Utility::removeChar(const string& str, char c) {
	string result;
	for (char currentChar : str)
		if (currentChar != c)
			result += currentChar;
	return result;
}

string Utility::extractChars(const string& str, char from, char to) {
	string result;
	if (not str.empty())
		for (char c : str)
			if (c >= from and c <= to)
				result.push_back(c);
	return result;
}

const string Utility::getHomeDir() {
	const char* homeDir = getenv("HOME");
	if (homeDir) {
		return string(homeDir);
	}
	return "/root";
}

const string Utility::getConfigDir() {
	return string(getHomeDir() + "/.local/share/" PACKAGE_NAME);
}

