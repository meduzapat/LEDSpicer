/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file		DataLoader.cpp
 * @since		Jun 22, 2018
 * @author		Patricio A. Rossi (MeduZa)
 * @copyright	Copyright © 2018 Patricio A. Rossi (MeduZa)
 */

#include "DataLoader.hpp"

using namespace LEDSpicer;
using namespace LEDSpicer::Devices;

void DataLoader::read() {

	umap<string, string> tempAttr = processNode(root);
	checkAttributes(REQUIRED_PARAM_ROOT, tempAttr, "root");

	ConnectionUSB::openSession(stoi(tempAttr["usbDebugLevel"]));

	processColorFile(string(PACKAGE_DATA_DIR).append("/").append(tempAttr["colorProfile"]).append(".xml"));

	processDevices();

	processLayout();
}

vector<Device*> DataLoader::getDevices() {
	return std::move(devices);
}

vector<Group> DataLoader::getLayout() {
	return std::move(layout);
}

vector<Animation*> DataLoader::getAnimations() {
	return std::move(animations);
}

void DataLoader::processColorFile(const string& file) {
	// Process colors file.
	XMLHelper colors(file);
	umap<string, string> tempAttr = processNode(colors.getRoot());
	tinyxml2::XMLElement* element = colors.getRoot()->FirstChildElement("color");
	if (not element)
		throw LEDError("No colors found");

	string format = tempAttr["format"];
	umap<string, string> colorsData;
	for (; element; element = element->NextSiblingElement()) {
		tempAttr = processNode(element);
		checkAttributes(REQUIRED_PARAM_COLOR, tempAttr, "color");
		if (colorsData.count(tempAttr["name"]))
			Log::warning("Duplicated color " + tempAttr["name"]);
		colorsData[tempAttr["name"]] = tempAttr["color"];
	}

	Color::loadColors(std::move(colorsData), format);
}

void DataLoader::processDevices() {

	umap<string, string> tempAttr;

	tinyxml2::XMLElement* devices = root->FirstChildElement("devices");
	if (not devices)
		throw LEDError("Missing devices section");

	tinyxml2::XMLElement* element = devices->FirstChildElement("device");
	if (not element)
		throw LEDError("Empty devices section");

	for (; element; element = element->NextSiblingElement("device")) {
		tempAttr = processNode(element);
		checkAttributes(REQUIRED_PARAM_DEVICE, tempAttr, "device");
		auto device = createDevice(tempAttr["name"], tempAttr["boardId"], tempAttr["fps"]);
		this->devices.push_back(device);
		processDeviceElements(element, device);
	}
}

void DataLoader::processDeviceElements(tinyxml2::XMLElement* deviceNode, Device* device) {

	umap<string, string> tempAttr;
	vector<bool> pinCheck(device->getNumberOfLeds(), false);

	tinyxml2::XMLElement* elements = deviceNode->FirstChildElement("elements");
	if (not elements)
		throw LEDError("Missing elements section in board " + device->getName());

	tinyxml2::XMLElement* element = elements->FirstChildElement("element");
	if (not element)
		throw LEDError("Empty elements section in board " + device->getName());

	for (; element; element = element->NextSiblingElement("element")) {
		Element deviceElement;
		tempAttr = processNode(element);
		checkAttributes(REQUIRED_PARAM_DEVICE_ELEMENT, tempAttr, "device element");

		vector<uint8_t> pins;
		// Single color.
		if (tempAttr.count("pin")) {
			pins.push_back(stoi(tempAttr["pin"]));
		}
		// RGB.
		else {
			checkAttributes(REQUIRED_PARAM_RGB_LED, tempAttr, tempAttr["name"]);
			pins.push_back(stoi(tempAttr["red"]));
			pins.push_back(stoi(tempAttr["green"]));
			pins.push_back(stoi(tempAttr["blue"]));
		}
		for (uint8_t pin : pins) {
			checkValidPin(pin, device->getNumberOfLeds(), device->getName());
			pinCheck[pin - 1] = true;
			deviceElement.pins.push_back(device->getLED(pin));
			deviceElement.name = tempAttr["name"];
		}

		this->elements[tempAttr["name"]] = std::move(deviceElement);
	}

	// Checks orphan Pins.
	Log::debug(to_string(this->elements.size()) + " elements with " + to_string(pinCheck.size()) + " LEDs");
	for (uint8_t pin = 0; pin < pinCheck.size(); ++pin) {
		if (not pinCheck[pin])
			Log::notice("Pin " + to_string(pin + 1) + " is orphan for device " + device->getName());
	}
}

void DataLoader::processLayout() {

	umap<string, string> tempAttr;

	tinyxml2::XMLElement* layouts = root->FirstChildElement("layout");
	if (not layouts)
		throw LEDError("Missing layout section");

	tempAttr = processNode(layouts);
	checkAttributes(REQUIRED_PARAM_LAYOUT, tempAttr, "layout");
//			tempAttr["defaultState"] == "Animation" ? Group::States::Animation : Group::States::Color,
//			tempAttr["stateValue"],

	tinyxml2::XMLElement* element = layouts->FirstChildElement("group");
	if (not element)
		throw LEDError("Empty layout section");

	for (; element; element = element->NextSiblingElement("group")) {
		tempAttr = processNode(element);
		checkAttributes(REQUIRED_PARAM_NAME_ONLY, tempAttr, "group");

		Group group(tempAttr["name"]);
		processGroupElement(element, group);
		layout.push_back(std::move(group));
	}
}

void DataLoader::processGroupElement(tinyxml2::XMLElement* groupNode, Group& group) {

	umap<string, string> tempAttr;

	tinyxml2::XMLElement* element = groupNode->FirstChildElement("element");

	if (not element)
		throw LEDError("Empty group section");

	for (; element; element = element->NextSiblingElement("element")) {
		tempAttr = processNode(element);
		checkAttributes(REQUIRED_PARAM_NAME_ONLY, tempAttr, "group element");
		// Check dupes and add
		for (auto el: group.elements)
			if (el.name == tempAttr["name"])
				throw LEDError("Duplicated element name " + tempAttr["name"] + " in group '" + group.name + "'");
		if (elements.count(tempAttr["name"]))
			group.elements.push_back(std::move(elements[tempAttr["name"]]));
		else
			throw LEDError("Invalid element " + tempAttr["name"] + " in group '" + group.name + "'");
	}
}

void DataLoader::processAnimation(const string& name) {


	XMLHelper animation(name);
	umap<string, string> actorData;
	tinyxml2::XMLElement* element = animation.getRoot()->FirstChildElement("actor");
	if (not element)
		throw LEDError("No actors found");

	for (; element; element = element->NextSiblingElement("actor")) {
		actorData = processNode(element);
		checkAttributes(REQUIRED_PARAM_COLOR, actorData, "actor for animation " + name);
		animations.push_back(createAnimation(name, actorData));
	}


}

Device* DataLoader::createDevice(const string& name, const string& boardId, const string& fps) {

	if (name == IPAC_ULTIMATE) {
		return new UltimarcUltimate(stoi(boardId), stoi(fps));
	}

	throw LEDError("Unknown board name");
}

Animation* createAnimation(const string& name, umap<string, string>& actorData) {
	string groupName = actorData["group"];
	if (actorData["type"] == "Serpentine") {
		return new Serpentine(std::move(actorData), *groups[groupName]);
	}
	if (actorData["type"] == "Pulse") {
		return new Pulse(std::move(actorData), *groups[groupName]);
	}
	throw LEDError("Invalid animation name " + name);
}


void DataLoader::checkValidPin(uint8_t pin, uint8_t pins, const string& name) {
	if (not pin or pin > pins)
		throw LEDError("Invalid pin number for " + name);
}

