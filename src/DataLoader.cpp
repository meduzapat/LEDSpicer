/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file		DataLoader.cpp
 * @since		Jun 22, 2018
 * @author		Patricio A. Rossi (MeduZa)
 * @copyright	Copyright © 2018 Patricio A. Rossi (MeduZa)
 */

#include "DataLoader.hpp"

using namespace LEDSpicer;

void DataLoader::read() {

	umap<string, string> tempAttr = processNode(root);
	Utility::checkAttributes(REQUIRED_PARAM_ROOT, tempAttr, "root");

	ConnectionUSB::openSession(stoi(tempAttr["usbDebugLevel"]));

	processColorFile(string(PACKAGE_DATA_DIR).append("/").append(tempAttr["colors"]).append(".xml"));

	processDevices();

	processLayout();
}

vector<Device*> DataLoader::getDevices() {
	return devices;
}

umap<string, Group> DataLoader::getLayout() {
	return layout;
}

string DataLoader::getDefaultProfile() const {
	return defaultProfile;
}

void DataLoader::processColorFile(const string& file) {
	// Process colors file.
	XMLHelper colors(file, "Colors");
	umap<string, string> colorAttr = processNode(colors.getRoot());

	tinyxml2::XMLElement* element = colors.getRoot()->FirstChildElement("color");
	if (not element)
		throw LEDError("No colors found");

	string format = colorAttr["format"];
	umap<string, string> colorsData;
	for (; element; element = element->NextSiblingElement()) {
		colorAttr = processNode(element);
		Utility::checkAttributes(REQUIRED_PARAM_COLOR, colorAttr, "color");
		if (colorsData.count(colorAttr["name"]))
			Log::warning("Duplicated color " + colorAttr["name"]);
		colorsData[colorAttr["name"]] = colorAttr["color"];
	}

	Color::loadColors(std::move(colorsData), format);
}

void DataLoader::processDevices() {

	tinyxml2::XMLElement* element = root->FirstChildElement("devices");
	if (not element)
		throw LEDError("Missing devices section");

	element = element->FirstChildElement("device");
	if (not element)
		throw LEDError("Empty devices section");

	for (; element; element = element->NextSiblingElement("device")) {
		umap<string, string> deviceAttr = processNode(element);
		Utility::checkAttributes(REQUIRED_PARAM_DEVICE, deviceAttr, "device");
		auto device = createDevice(deviceAttr["name"], deviceAttr["boardId"], deviceAttr["fps"]);
		this->devices.push_back(device);
		processDeviceElements(element, device);
	}
}

void DataLoader::processDeviceElements(tinyxml2::XMLElement* deviceNode, Device* device) {

	vector<bool> pinCheck(device->getNumberOfLeds(), false);

	tinyxml2::XMLElement* element = deviceNode->FirstChildElement("element");
	if (not element)
		throw LEDError("Empty elements section in board " + device->getName());

	for (; element; element = element->NextSiblingElement("element")) {
		Element deviceElement;
		umap<string, string> tempAttr = processNode(element);
		Utility::checkAttributes(REQUIRED_PARAM_DEVICE_ELEMENT, tempAttr, "device element");

		vector<uint8_t> pins;
		// Single color.
		if (tempAttr.count("pin")) {
			pins.push_back(stoi(tempAttr["pin"]));
		}
		// RGB.
		else {
			Utility::checkAttributes(REQUIRED_PARAM_RGB_LED, tempAttr, tempAttr["name"]);
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

	tinyxml2::XMLElement* layout = root->FirstChildElement("layout");
	if (not layout)
		throw LEDError("Missing layout section");

	tempAttr = processNode(layout);
	Utility::checkAttributes(REQUIRED_PARAM_LAYOUT, tempAttr, "layout");
	defaultProfile = tempAttr["defaultProfile"];

	tinyxml2::XMLElement* element = layout->FirstChildElement("group");
	if (not element)
		throw LEDError("Empty layout section");

	for (; element; element = element->NextSiblingElement("group")) {
		tempAttr = processNode(element);
		Utility::checkAttributes(REQUIRED_PARAM_NAME_ONLY, tempAttr, "group");

		if (this->layout.count(tempAttr["name"]))
			throw LEDError("Duplicated group " + tempAttr["name"]);

		Group group;
		processGroupElements(element, group, tempAttr["name"]);
		this->layout[tempAttr["name"]] = std::move(group);
	}
}

void DataLoader::processGroupElements(tinyxml2::XMLElement* groupNode, Group& group, const string& name) {

	umap<string, string> elementAttr;

	tinyxml2::XMLElement* element = groupNode->FirstChildElement("element");

	if (not element)
		throw LEDError("Empty group section");

	for (; element; element = element->NextSiblingElement("element")) {
		elementAttr = processNode(element);
		Utility::checkAttributes(REQUIRED_PARAM_NAME_ONLY, elementAttr, "group element");
		// Check dupes and add
		for (auto el: group.elements)
			if (el.name == elementAttr["name"])
				throw LEDError("Duplicated element name " + elementAttr["name"] + " in group '" + name + "'");
		if (elements.count(elementAttr["name"]))
			group.elements.push_back(std::move(elements[elementAttr["name"]]));
		else
			throw LEDError("Invalid element " + elementAttr["name"] + " in group '" + name + "'");
	}
}

Profile* DataLoader::processProfile(const string& name) {

	XMLHelper profile(string(PACKAGE_DATA_DIR).append("/profiles/").append(name).append(".xml"), "Profile");
	umap<string, string> tempAttr = processNode(profile.getRoot());
	Utility::checkAttributes(REQUIRED_PARAM_PROFILE, tempAttr, "root");

	Profile* profilePtr = new Profile(
		Color(tempAttr.at("defaultColorOn")),
		Color(tempAttr.at("defaultColorOff")),
		processAnimation(tempAttr["startAnimation"]),
		processAnimation(tempAttr["stopAnimation"])
	);

	// Check for animations.
	tinyxml2::XMLElement* element = profile.getRoot()->FirstChildElement("animations");
	if (element) {
		element = element->FirstChildElement("animation");
		for (; element; element = element->NextSiblingElement("animation")) {
			tempAttr = processNode(element);
			Utility::checkAttributes(REQUIRED_PARAM_NAME_ONLY, tempAttr, "animations for profile " + name);
			profilePtr->addAnimation(tempAttr["name"], processAnimation(tempAttr["name"]));
		}
	}
	return profilePtr;
}

vector<Actor*> DataLoader::processAnimation(const string& name) {

	// TODO cache information on disk about animations so we avoid a read from HD.
	XMLHelper animation(string(PACKAGE_DATA_DIR).append("/animations/").append(name).append(".xml"), "Animation");

	umap<string, string> actorData;
	tinyxml2::XMLElement* element = animation.getRoot()->FirstChildElement("actor");
	if (not element)
		throw LEDError("No actors found");

	vector<Actor*> actors;
	for (; element; element = element->NextSiblingElement("actor")) {
		actorData = processNode(element);
		Utility::checkAttributes(REQUIRED_PARAM_ACTOR, actorData, "actor for animation " + name);
		actors.push_back(createAnimation(name, actorData));
	}

	return actors;
}

Device* DataLoader::createDevice(const string& name, const string& boardId, const string& fps) {

	if (name == IPAC_ULTIMATE) {
		return new UltimarcUltimate(stoi(boardId), stoi(fps));
	}

	throw LEDError("Unknown board name");
}

Actor* DataLoader::createAnimation(const string& name, umap<string, string>& actorData) {
	string groupName = actorData["group"];
	if (actorData["type"] == "Serpentine") {
		Utility::checkAttributes(REQUIRED_PARAM_ACTOR_SERPENTINE, actorData, "animation Serpentine");
		return new Serpentine(actorData, layout[groupName]);
	}
	if (actorData["type"] == "Pulse") {
		Utility::checkAttributes(REQUIRED_PARAM_ACTOR_PULSE, actorData, "animation Pulse");
		return new Pulse(actorData, layout[groupName]);
	}
	throw LEDError("Invalid animation type '" + actorData["type"] + "' inside " + name);
}


void DataLoader::checkValidPin(uint8_t pin, uint8_t pins, const string& name) {
	if (not pin or pin > pins)
		throw LEDError("Invalid pin number for " + name);
}

