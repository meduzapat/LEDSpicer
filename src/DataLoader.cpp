/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DataLoader.cpp
 * @since     Jun 22, 2018
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2018 - 2019 Patricio A. Rossi (MeduZa)
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

#include "DataLoader.hpp"

using namespace LEDSpicer;

vector<Device*> DataLoader::devices;
umap<string, Element*> DataLoader::allElements;
umap<string, Group> DataLoader::layout;
Profile* DataLoader::defaultProfile;
umap<string, Profile*> DataLoader::profiles;
string DataLoader::portNumber;
umap<string, DeviceHandler*> DataLoader::deviceHandlers;
umap<Device*, DeviceHandler*> DataLoader::deviceMap;
umap<string, ActorHandler*> DataLoader::actorHandlers;
umap<Actor*, ActorHandler*> DataLoader::actorMap;
DataLoader::Modes DataLoader::mode = DataLoader::Modes::Normal;

DataLoader::Modes DataLoader::getMode() {
	return mode;
}

void DataLoader::setMode(Modes mode) {
	DataLoader::mode = mode;
}

void DataLoader::readConfiguration() {

	umap<string, string> tempAttr = processNode(root);
	Utility::checkAttributes(REQUIRED_PARAM_ROOT, tempAttr, "root");

	// set log level.
	if ((mode != Modes::Dump or mode != Modes::Profile) and tempAttr.count("logLevel"))
		Log::setLogLevel(Log::str2level(tempAttr["logLevel"]));

	// set FPS.
	uint8_t fps = Utility::parseNumber(tempAttr["fps"], "Invalid value for FPS");
	if (fps == 0)
		throw LEDError("FPS = 0, No speed, nothing to do, done");
	ConnectionUSB::setInterval(1000 / (fps > 30 ? 30 : fps));
	// activate LIBUSB.
	ConnectionUSB::openSession();
	Actor::setFPS((fps > 30 ? 30 : fps));

	portNumber = tempAttr["port"];

	processColorFile(createFilename(tempAttr["colors"]));

	processDevices();

	processLayout();
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
			LogWarning("Duplicated color " + colorAttr["name"]);
		colorsData[colorAttr["name"]] = colorAttr["color"];
	}

	Color::loadColors(colorsData, format);
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
		auto device = createDevice(deviceAttr);
		LogInfo("Initializing " + device->getFullName());
		this->devices.push_back(device);
		processDeviceElements(element, device);
	}
}

void DataLoader::processDeviceElements(tinyxml2::XMLElement* deviceNode, Device* device) {

	vector<bool> pinCheck(device->getNumberOfLeds(), false);

	tinyxml2::XMLElement* element = deviceNode->FirstChildElement("element");
	if (not element)
		throw LEDError("Empty elements section in " + device->getFullName());

	for (; element; element = element->NextSiblingElement("element")) {
		umap<string, string> tempAttr = processNode(element);

		if (allElements.count(tempAttr["name"]))
			throw LEDError("Duplicated element " + tempAttr["name"]);

		Utility::checkAttributes(REQUIRED_PARAM_NAME_ONLY, tempAttr, "device element");

		// Single color.
		if (tempAttr.count("led")) {
			uint8_t pin = Utility::parseNumber(tempAttr["led"], "Invalid Value for pin in " + device->getFullName()) - 1;
			device->registerElement(tempAttr["name"], pin);
			pinCheck[pin] = true;
		}
		// RGB.
		else {
			Utility::checkAttributes(REQUIRED_PARAM_RGB_LED, tempAttr, tempAttr["name"]);
			uint8_t
				r = Utility::parseNumber(tempAttr["red"], "Invalid Value for red pin in " + device->getFullName()) - 1,
				g = Utility::parseNumber(tempAttr["green"], "Invalid Value for green pin in " + device->getFullName()) - 1,
				b = Utility::parseNumber(tempAttr["blue"], "Invalid Value for blue pin in " + device->getFullName()) - 1;
			device->registerElement(tempAttr["name"], r, g , b);
			pinCheck[r] = true;
			pinCheck[g] = true;
			pinCheck[b] = true;
		}
		allElements.emplace(tempAttr["name"], device->getElement(tempAttr["name"]));
	}

	// Checks orphan Pins.
	LogInfo(
		device->getFullName() + " with " +
		to_string(pinCheck.size()) + " LEDs divided into " +
		to_string(device->getNumberOfElements()) + " elements"
	);
	for (uint8_t pin = 0; pin < pinCheck.size(); ++pin)
		if (not pinCheck[pin])
			LogInfo("Pin " + to_string(pin + 1) + " is not set for " + device->getFullName());
}

void DataLoader::processLayout() {

	umap<string, string> tempAttr;

	tinyxml2::XMLElement* layoutNode = root->FirstChildElement("layout");
	if (not layoutNode)
		throw LEDError("Missing layout section");

	tempAttr = processNode(layoutNode);
	Utility::checkAttributes(REQUIRED_PARAM_LAYOUT, tempAttr, "layout");
	string defaultProfileStr = tempAttr["defaultProfile"];

	tinyxml2::XMLElement* element = layoutNode->FirstChildElement("group");
	if (not element)
		throw LEDError("Empty layout section");

	for (; element; element = element->NextSiblingElement("group")) {
		tempAttr = processNode(element);
		Utility::checkAttributes(REQUIRED_PARAM_NAME_ONLY, tempAttr, "group");

		if (this->layout.count(tempAttr["name"]))
			throw LEDError("Duplicated group " + tempAttr["name"]);

		layout.emplace(tempAttr["name"], Group(tempAttr["name"]));
		processGroupElements(element, layout[tempAttr["name"]]);
	}

	// Create a group with all elements on it called All if not defined.
	if (not this->layout.count("All")) {
		Group group("All");
		for (auto& gp : allElements)
			group.linkElement(gp.second);
		group.shrinkToFit();
		layout.emplace("All", group);
	}

	defaultProfile = processProfile(defaultProfileStr);

}

void DataLoader::processGroupElements(tinyxml2::XMLElement* groupNode, Group& group) {

	tinyxml2::XMLElement* element = groupNode->FirstChildElement("element");

	if (not element)
		throw LEDError("Empty group section");

	umap<string, bool> groupElements;
	for (; element; element = element->NextSiblingElement("element")) {
		umap<string, string> elementAttr = processNode(element);
		Utility::checkAttributes(REQUIRED_PARAM_NAME_ONLY, elementAttr, "group element");

		// Check dupes and add
		if (groupElements.count(elementAttr["name"]))
			throw LEDError("Duplicated element name " + elementAttr["name"] + " in group '" + group.getName() + "'");

		if (allElements.count(elementAttr["name"])) {
			group.linkElement(allElements[elementAttr["name"]]);
			groupElements.emplace(elementAttr["name"], true);
		}
		else {
			throw LEDError("Invalid element " + elementAttr["name"] + " in group '" + group.getName() + "'");
		}
	}
	group.shrinkToFit();
}

Profile* DataLoader::processProfile(const string& name) {

	if (profiles.count(name))
		return profiles[name];

	XMLHelper profile(createFilename(PROFILE_DIR + name), "Profile");
	umap<string, string> tempAttr = processNode(profile.getRoot());
	Utility::checkAttributes(REQUIRED_PARAM_PROFILE, tempAttr, "root");

	Actor
	 * start = nullptr,
	 * end   = nullptr;

	tinyxml2::XMLElement* element = profile.getRoot()->FirstChildElement("startTransition");
	if (element) {
		umap<string, string> tempAttr = processNode(element);
		tempAttr["group"]  = "All";
		tempAttr["filter"] = "Normal";
		start = createAnimation(tempAttr);
	}

	element = profile.getRoot()->FirstChildElement("endTransition");
	if (element) {
		umap<string, string> tempAttr = processNode(element);
		tempAttr["group"]  = "All";
		tempAttr["filter"] = "Normal";
		end = createAnimation(tempAttr);
	}

	Profile* profilePtr = new Profile(
		name,
		Color(tempAttr.at("backgroundColor")),
		start,
		end
	);

	// Check for animations.
	element = profile.getRoot()->FirstChildElement("animations");
	if (element) {
		element = element->FirstChildElement("animation");
		for (; element; element = element->NextSiblingElement("animation")) {
			tempAttr = processNode(element);
			Utility::checkAttributes(REQUIRED_PARAM_NAME_ONLY, tempAttr, "animations for profile " + name);
			profilePtr->addAnimation(processAnimation(tempAttr["name"]));
		}
	}
	// Check for always on elements.
	element = profile.getRoot()->FirstChildElement("alwaysOnElements");
	if (element) {
		element = element->FirstChildElement("element");
		for (; element; element = element->NextSiblingElement("element")) {
			tempAttr = processNode(element);
			Utility::checkAttributes(REQUIRED_PARAM_COLOR, tempAttr, "alwaysOnElements for profile " + name);
			if (not allElements.count(tempAttr["name"]))
				throw LEDError("Unknown element [" + tempAttr["name"] + "] for always on element on profile " + name);
			profilePtr->addAlwaysOnElement(allElements[tempAttr["name"]], tempAttr["color"]);
		}
	}
	// Check for always on elements.
	element = profile.getRoot()->FirstChildElement("alwaysOnGroups");
	if (element) {
		element = element->FirstChildElement("group");
		for (; element; element = element->NextSiblingElement("group")) {
			tempAttr = processNode(element);
			Utility::checkAttributes(REQUIRED_PARAM_COLOR, tempAttr, "alwaysOnGroups for profile " + name);
			if (not layout.count(tempAttr["name"]))
				throw LEDError("Unknown group [" + tempAttr["name"] + "] for always on group on profile " + name);
			profilePtr->addAlwaysOnGroup(&layout[tempAttr["name"]], tempAttr["color"]);
		}
	}
	profiles[name] = profilePtr;
	return profilePtr;
}

vector<Actor*> DataLoader::processAnimation(const string& name) {

	XMLHelper animation(createFilename(ACTOR_DIR + name), "Animation");

	umap<string, string> actorData;
	tinyxml2::XMLElement* element = animation.getRoot()->FirstChildElement("actor");
	if (not element)
		throw LEDError("No actors found");

	vector<Actor*> actors;
	for (; element; element = element->NextSiblingElement("actor")) {
		actorData = processNode(element);
		Utility::checkAttributes(REQUIRED_PARAM_ACTOR, actorData, "actor for animation " + name);
		actors.push_back(createAnimation(actorData));
	}

	return actors;
}

Device* DataLoader::createDevice(umap<string, string>& deviceData) {

	uint8_t devId = deviceData.count("boardId") ? Utility::parseNumber(deviceData["boardId"], "Device id should be a number") : 1;
	string deviceName = deviceData["name"];
	if (not deviceHandlers.count(deviceName))
		deviceHandlers.emplace(deviceName, new DeviceHandler(deviceName));
	Device* d = deviceHandlers[deviceName]->createDevice(devId, deviceData);
	deviceMap.emplace(d, deviceHandlers[deviceName]);
	return d;
}

Actor* DataLoader::createAnimation(umap<string, string>& actorData) {

	string
		groupName = actorData["group"],
		actorName = actorData["type"];

	if (not layout.count(groupName))
		throw LEDError(groupName + " is not a valid group name in animation " + actorName);

	if (not actorHandlers.count(actorName))
		actorHandlers.emplace(actorName, new ActorHandler(actorName));
	Actor* a = actorHandlers[actorName]->createActor(actorData, &layout.at(groupName));
	actorMap.emplace(a, actorHandlers[actorName]);
	return a;
}

string DataLoader::createFilename(const string& name) {
	return (
		string(PACKAGE_DATA_DIR)
			.append(Utility::removeChar(name, '.'))
			.append(".xml")
	);
}
