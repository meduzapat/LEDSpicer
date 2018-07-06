/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file		Utility.cpp
 * @since		Jun 24, 2018
 * @author		Patricio A. Rossi (MeduZa)
 * @copyright	Copyright © 2018 Patricio A. Rossi (MeduZa)
 */

#include "Utility.hpp"

using namespace LEDSpicer;

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

	return std::move(temp);
}

bool Utility::verifyValue(int& val, int min, int max, bool truncate) {
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

bool Utility::verifyValue(uint8_t& val, uint8_t min, uint8_t max, bool truncate) {
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
