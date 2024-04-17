/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      FileReader.cpp
 * @since     Nov 14, 2019
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

#include <cmath>
#include "utility/XMLHelper.hpp"
#include "FileReader.hpp"

using namespace LEDSpicer::Animations;

umap<string, vector<vector<LEDSpicer::Color>>> FileReader::fileData;

FileReader::FileReader(umap<string, string>& parameters, Group* const group) :
	StepActor(parameters, group, REQUIRED_PARAM_ACTOR_FILEREADER)
{
	auto found = fileData.find(parameters["filename"]);
	if (found == fileData.end()) {
		switch(str2Format(parameters["format"])) {
		case Formats::rgba:
			processRGBA(parameters["filename"]);
			break;
		}
	}
	else{
		LogDebug("File " + parameters["filename"] + " already on memory.");
		frames = found;
	}
	totalStepFrames /= 2;
	totalFrames = frames->second.size() - 1;
}

void FileReader::drawConfig() {
	cout
		<< "Type: FileReader " << endl
		<< "File: " << frames->first << endl;
	DirectionActor::drawConfig();
}

string FileReader::Format2str(Formats format) {
	switch (format) {
	case Formats::rgba:
		return "rgba";
	}
	return "";
}

FileReader::Formats FileReader::str2Format(const string& format) {
	if (format == "rgba")
		return Formats::rgba;
	throw Error("Invalid type " + format);
}

void FileReader::processRGBA(const string& filename) {
	XMLHelper file(filename, "");
	umap<string, string> fileAttr = file.processNode(file.getRoot());
	tinyxml2::XMLElement * element = file.getRoot()->FirstChildElement("frm");
	if (not element)
		throw LEDError("No frames found for " + filename);
	vector<vector<Color>> framesTmp;
	for (; element; element = element->NextSiblingElement("frm")) {
		umap<string, string> frameData = file.processNode(element);
		Utility::checkAttributes({"dec"}, frameData, "frm");
		vector<string> colorData = Utility::explode(frameData["dec"], ',');
		uint16_t ceil = std::ceil(colorData.size() / 3.00);
		if (colorData.empty()) {
			LogWarning("Empty Frame detected");
			continue;
		}
		else {
			LogDebug("Extracting frame with " + to_string(colorData.size()) + " colors (" + to_string(ceil) + " elements" + (colorData.size() % 3 ? " needs " + to_string(ceil * 3) + " colors" : "") + ")");
		}
#ifdef DEVELOP
		if (ceil > getNumberOfElements()) {
			LogWarning("More colors than element colors detected");
		}
#endif
		vector<Color> frameTmp;
		Color colorTmp;
		for (uint16_t c = 0; c < colorData.size();) {
			for (uint8_t c2 = 0; c2 < 3; ++c2, ++c) {
				if (c == colorData.size())
					break;
				uint8_t color = std::stoi(colorData[c]);
				switch (c2) {
				case 0:
					colorTmp.setR(color);
					break;
				case 1:
					colorTmp.setG(color);
					break;
				case 2:
					colorTmp.setB(color);
					break;
				}
			}
			frameTmp.push_back(std::move(colorTmp));
			if (frameTmp.size() == getNumberOfElements())
				break;
		}
		frameTmp.shrink_to_fit();
		framesTmp.push_back(std::move(frameTmp));
	}
	fileData.emplace(filename, framesTmp);
	frames = fileData.find(filename);
}

void FileReader::calculateElements() {
#ifdef DEVELOP
	cout << "FileReader: " << DrawDirection(cDirection) << " F: " << (currentFrame + 1) << endl;
#endif
	for (uint8_t c = 0; c < frames->second[currentFrame].size(); ++c) {
#ifdef DEVELOP
		cout << "element: " <<  to_string(c)  << " -> ";
		frames->second[currentFrame].at(c).drawHex();
		cout << endl;
#endif
		changeElementColor(c, frames->second[currentFrame].at(c), filter);
	}
}
