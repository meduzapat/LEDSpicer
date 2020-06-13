/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DataLoader.cpp
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

#include "DataLoader.hpp"

using namespace LEDSpicer;

vector<Device*> DataLoader::devices;
umap<string, Element*> DataLoader::allElements;
umap<string, Group> DataLoader::layout;
Profile* DataLoader::defaultProfile;
umap<string, Profile*> DataLoader::profiles;
string DataLoader::portNumber;
umap<string, DeviceHandler*> DataLoader::deviceHandlers;
umap<string, ActorHandler*> DataLoader::actorHandlers;
umap<string, InputHandler*> DataLoader::inputHandlers;
DataLoader::Modes DataLoader::mode = DataLoader::Modes::Normal;
umap<string, Items*> DataLoader::controlledItems;
milliseconds DataLoader::waitTime;

DataLoader::Modes DataLoader::getMode() {
	return mode;
}

void DataLoader::setMode(Modes mode) {
	DataLoader::mode = mode;
}

void DataLoader::readConfiguration() {

	Input::setInputControllers(&controlledItems);

	umap<string, string> tempAttr = processNode(root);
	Utility::checkAttributes(REQUIRED_PARAM_ROOT, tempAttr, "root");

	// set log level.
	if ((mode != Modes::Dump or mode != Modes::Profile) and tempAttr.count(PARAM_LOG_LEVEL))
		Log::setLogLevel(Log::str2level(tempAttr[PARAM_LOG_LEVEL]));

	// set FPS.
	uint8_t fps = Utility::parseNumber(tempAttr[PARAM_FPS], "Invalid value for FPS");
	if (fps == 0)
		throw LEDError("FPS = 0, No speed, nothing to do, done");
	setInterval(1000 / (fps > MAXIMUM_FPS ? MAXIMUM_FPS : fps));

	Actor::setFPS((fps > MAXIMUM_FPS ? MAXIMUM_FPS : fps));

	portNumber = tempAttr[PARAM_PORT];

	processColorFile(createFilename(tempAttr[PARAM_COLORS]));

	processDevices();

	processLayout();

#ifndef DRY_RUN
	for (auto device : devices)
		device->initialize();
#endif
	dropRootPrivileges(
		static_cast<uid_t>(Utility::parseNumber(tempAttr[PARAM_USER_ID], "Invalid value for user ID")),
		static_cast<gid_t>(Utility::parseNumber(tempAttr[PARAM_GROUP_ID], "Invalid value for group ID"))
	);
}

void DataLoader::processColorFile(const string& file) {
	// Process colors file.
	XMLHelper colors(file, "Colors");
	umap<string, string> colorAttr = processNode(colors.getRoot());

	tinyxml2::XMLElement* element = colors.getRoot()->FirstChildElement(NODE_COLOR);
	if (not element)
		throw LEDError("No colors found");

	string format = colorAttr["format"];
	umap<string, string> colorsData;
	for (; element; element = element->NextSiblingElement()) {
		colorAttr = processNode(element);
		Utility::checkAttributes(REQUIRED_PARAM_COLOR, colorAttr, NODE_COLOR);
		if (colorsData.count(colorAttr[PARAM_NAME]))
			LogWarning("Duplicated color " + colorAttr[PARAM_NAME]);
		colorsData[colorAttr[PARAM_NAME]] = colorAttr[PARAM_COLOR];
	}

	Color::loadColors(colorsData, format);
}

void DataLoader::processDevices() {

	tinyxml2::XMLElement* element = root->FirstChildElement(NODE_DEVICES);
	if (not element)
		throw LEDError("Missing " NODE_DEVICES " section");

	element = element->FirstChildElement(NODE_DEVICE);
	if (not element)
		throw LEDError("Empty " NODE_DEVICES " section");

	for (; element; element = element->NextSiblingElement(NODE_DEVICE)) {
		umap<string, string> deviceAttr = processNode(element);
		Utility::checkAttributes(REQUIRED_PARAM_DEVICE, deviceAttr, NODE_DEVICE);
		auto device = createDevice(deviceAttr);
		LogInfo("Processing " + device->getFullName());
		this->devices.push_back(device);
		processDeviceElements(element, device);
	}
}

void DataLoader::processDeviceElements(tinyxml2::XMLElement* deviceNode, Device* device) {

	vector<bool> pinCheck(device->getNumberOfLeds(), false);

	tinyxml2::XMLElement* element = deviceNode->FirstChildElement(NODE_ELEMENT);
	if (not element)
		throw LEDError("Empty elements section in " + device->getFullName());

	for (; element; element = element->NextSiblingElement(NODE_ELEMENT)) {
		umap<string, string> tempAttr = processNode(element);

		if (allElements.count(tempAttr[PARAM_NAME]))
			throw LEDError("Duplicated element " + tempAttr[PARAM_NAME]);

		Utility::checkAttributes(REQUIRED_PARAM_NAME_ONLY, tempAttr, "device element");

		// Single color.
		if (tempAttr.count("led")) {
			uint8_t pin = Utility::parseNumber(tempAttr[PARAM_LED], "Invalid Value for pin in " + device->getFullName()) - 1;
			device->registerElement(tempAttr[PARAM_NAME], pin);
			pinCheck[pin] = true;
		}
		// RGB.
		else {
			Utility::checkAttributes(REQUIRED_PARAM_RGB_LED, tempAttr, tempAttr[PARAM_NAME]);
			uint8_t
				r = Utility::parseNumber(tempAttr[PARAM_RED], "Invalid Value for red pin in " + device->getFullName()) - 1,
				g = Utility::parseNumber(tempAttr[PARAM_GREEN], "Invalid Value for green pin in " + device->getFullName()) - 1,
				b = Utility::parseNumber(tempAttr[PARAM_BLUE], "Invalid Value for blue pin in " + device->getFullName()) - 1;
			device->registerElement(tempAttr[PARAM_NAME], r, g , b);
			pinCheck[r] = true;
			pinCheck[g] = true;
			pinCheck[b] = true;
		}
		allElements.emplace(tempAttr[PARAM_NAME], device->getElement(tempAttr[PARAM_NAME]));
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

	tinyxml2::XMLElement* layoutNode = root->FirstChildElement(NODE_LAYOUT);
	if (not layoutNode)
		throw LEDError("Missing " NODE_LAYOUT " section");

	tempAttr = processNode(layoutNode);
	Utility::checkAttributes(REQUIRED_PARAM_LAYOUT, tempAttr, NODE_LAYOUT);
	string defaultProfileStr = tempAttr[PARAM_DEFAULT_PROFILE];

	tinyxml2::XMLElement* element = layoutNode->FirstChildElement(NODE_GROUP);
	if (element)
		for (; element; element = element->NextSiblingElement(NODE_GROUP)) {
			tempAttr = processNode(element);
			Utility::checkAttributes(REQUIRED_PARAM_NAME_ONLY, tempAttr, "group");

			if (this->layout.count(tempAttr[PARAM_NAME]))
				throw LEDError("Duplicated group " + tempAttr[PARAM_NAME]);

			layout.emplace(tempAttr[PARAM_NAME], Group(tempAttr[PARAM_NAME]));
			processGroupElements(element, layout[tempAttr[PARAM_NAME]]);
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

	tinyxml2::XMLElement* element = groupNode->FirstChildElement(NODE_ELEMENT);

	if (not element)
		throw LEDError("Empty group section");

	umap<string, bool> groupElements;
	for (; element; element = element->NextSiblingElement(NODE_ELEMENT)) {
		umap<string, string> elementAttr = processNode(element);
		Utility::checkAttributes(REQUIRED_PARAM_NAME_ONLY, elementAttr, "group element");

		// Check dupes and add
		if (groupElements.count(elementAttr[PARAM_NAME]))
			throw LEDError("Duplicated element name " + elementAttr[PARAM_NAME] + " in group '" + group.getName() + "'");

		if (allElements.count(elementAttr[PARAM_NAME])) {
			group.linkElement(allElements[elementAttr[PARAM_NAME]]);
			groupElements.emplace(elementAttr[PARAM_NAME], true);
		}
		else {
			throw LEDError("Invalid element " + elementAttr[PARAM_NAME] + " in group '" + group.getName() + "'");
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

	FrameActor
	 * start = nullptr,
	 * end   = nullptr;

	tinyxml2::XMLElement* element = profile.getRoot()->FirstChildElement(NODE_START_TRANSITION);
	if (element) {
		umap<string, string> tempAttr = processNode(element);
		tempAttr["group"]  = "All";
		tempAttr["filter"] = "Normal";
		tempAttr["cycles"] = "1";
		start = dynamic_cast<FrameActor*>(createAnimation(tempAttr));
		if (not start)
			LogWarning("Actor " + tempAttr["type"] + " cannot be used as a start transition");
	}

	element = profile.getRoot()->FirstChildElement(NODE_END_TRANSITION);
	if (element) {
		umap<string, string> tempAttr = processNode(element);
		tempAttr["group"]  = "All";
		tempAttr["filter"] = "Normal";
		tempAttr["cycles"] = "1";
		end = dynamic_cast<FrameActor*>(createAnimation(tempAttr));
		if (not end)
			LogWarning("Actor " + tempAttr["type"] + " cannot be used as an end transition");
	}

	Profile* profilePtr = new Profile(
		name,
		Color(tempAttr["backgroundColor"]),
		start,
		end
	);

	// Check for animations.
	element = profile.getRoot()->FirstChildElement(NODE_ANIMATIONS);
	if (element) {
		element = element->FirstChildElement(NODE_ANIMATION);
		for (; element; element = element->NextSiblingElement(NODE_ANIMATION)) {
			tempAttr = processNode(element);
			Utility::checkAttributes(REQUIRED_PARAM_NAME_ONLY, tempAttr, "animations for profile " + name);
			profilePtr->addAnimation(processAnimation(tempAttr[PARAM_NAME]));
		}
	}
	// Check for always on elements.
	element = profile.getRoot()->FirstChildElement("alwaysOnElements");
	if (element) {
		element = element->FirstChildElement(NODE_ELEMENT);
		for (; element; element = element->NextSiblingElement(NODE_ELEMENT)) {
			tempAttr = processNode(element);
			Utility::checkAttributes(REQUIRED_PARAM_COLOR, tempAttr, "alwaysOnElements for profile " + name);
			if (not allElements.count(tempAttr[PARAM_NAME]))
				throw LEDError("Unknown element [" + tempAttr[PARAM_NAME] + "] for always on element on profile " + name);
			profilePtr->addAlwaysOnElement(allElements[tempAttr[PARAM_NAME]], tempAttr[PARAM_COLOR]);
		}
	}

	// Check for always on groups.
	element = profile.getRoot()->FirstChildElement("alwaysOnGroups");
	if (element) {
		element = element->FirstChildElement(NODE_GROUP);
		for (; element; element = element->NextSiblingElement(NODE_GROUP)) {
			tempAttr = processNode(element);
			Utility::checkAttributes(REQUIRED_PARAM_COLOR, tempAttr, "alwaysOnGroups for profile " + name);
			if (not layout.count(tempAttr[PARAM_NAME]))
				throw LEDError("Unknown group [" + tempAttr[PARAM_NAME] + "] for always on group on profile " + name);
			profilePtr->addAlwaysOnGroup(&layout[tempAttr[PARAM_NAME]], tempAttr[PARAM_COLOR]);
		}
	}

	// Check for input plugins.
	element = profile.getRoot()->FirstChildElement(NODE_INPUTS);
	if (element) {
		element = element->FirstChildElement(NODE_INPUT);
		if (element) {
			for (; element; element = element->NextSiblingElement(NODE_INPUT)) {
				umap<string, string> elementAttr = processNode(element);
				Utility::checkAttributes(REQUIRED_PARAM_NAME_ONLY, elementAttr, "Inputs");
				processInput(profilePtr, elementAttr[PARAM_NAME]);
			}
		}
	}

	profiles[name] = profilePtr;
	return profilePtr;
}

Device* DataLoader::createDevice(umap<string, string>& deviceData) {

	uint8_t devId = deviceData.count("boardId") ? Utility::parseNumber(deviceData["boardId"], "Device id should be a number") : 1;
	string deviceName = deviceData["name"];
	if (not deviceHandlers.count(deviceName))
		deviceHandlers.emplace(deviceName, new DeviceHandler(deviceName));
	return deviceHandlers[deviceName]->createDevice(devId, deviceData);
}

vector<Actor*> DataLoader::processAnimation(const string& file) {

	XMLHelper animation(createFilename(ACTOR_DIR + file), "Animation");

	umap<string, string> actorData;
	tinyxml2::XMLElement* element = animation.getRoot()->FirstChildElement(NODE_ACTOR);
	if (not element)
		throw LEDError("No actors found");

	vector<Actor*> actors;
	for (; element; element = element->NextSiblingElement(NODE_ACTOR)) {
		actorData = processNode(element);
		Utility::checkAttributes(REQUIRED_PARAM_ACTOR, actorData, "actor for animation " + file);
		actors.push_back(createAnimation(actorData));
	}

	return actors;
}

Actor* DataLoader::createAnimation(umap<string, string>& actorData) {

	string
		groupName = actorData["group"],
		actorName = actorData["type"];

	if (not layout.count(groupName))
		throw LEDError(groupName + " is not a valid group name in animation " + actorName);

	if (not actorHandlers.count(actorName))
		actorHandlers.emplace(actorName, new ActorHandler(actorName));
	return actorHandlers[actorName]->createActor(actorData, &layout.at(groupName));
}

void DataLoader::processInput(Profile* profile, const string& file) {
	XMLHelper inputFile(createFilename(INPUT_DIR + file), "Input");
	umap<string, string> elementAttr = processNode(inputFile.getRoot());
	Utility::checkAttributes(REQUIRED_PARAM_NAME_ONLY, elementAttr, file);
	string inputName = elementAttr[PARAM_NAME];
	if (not inputHandlers.count(inputName))
		inputHandlers.emplace(inputName, new InputHandler(inputName));
	umap<string, Items*> inputMapTmp = processInputMap(inputFile.getRoot());
	profile->addInput(inputHandlers[inputName]->createInput(file, elementAttr, inputMapTmp));
}

umap<string, Items*> DataLoader::processInputMap(tinyxml2::XMLElement* inputNode) {

	umap<string, Items*> itemsMap;

	tinyxml2::XMLElement* maps = inputNode->FirstChildElement("map");
	for (; maps; maps = maps->NextSiblingElement("map")) {
		umap<string, string> elementAttr = processNode(maps);
		Utility::checkAttributes(REQUIRED_PARAM_MAP, elementAttr, "map maps");

		if (elementAttr["type"] == "Element") {
			if (not allElements.count(elementAttr["target"]))
				throw LEDError("Unknown Element " + elementAttr["target"] + " on map file");

			// Check dupes and add
			if (itemsMap.count(elementAttr["trigger"]) and itemsMap[elementAttr["trigger"]]->getName() == elementAttr["target"])
				throw LEDError("Duplicated element map for " + elementAttr["target"] + " map " + elementAttr["value"]);

			itemsMap.emplace(elementAttr["trigger"], new Element::Item(
				allElements[elementAttr["target"]],
				&Color::getColor(elementAttr[PARAM_COLOR]),
				Color::str2filter(elementAttr["filter"])
			));
		}

		if (elementAttr["type"] == "Group") {
			if (not layout.count(elementAttr["target"]))
				throw LEDError("Unknown Group " + elementAttr["target"] + " on map file");

			// Check dupes and add
			if (itemsMap.count(elementAttr["trigger"]) and itemsMap[elementAttr["trigger"]]->getName() == elementAttr["target"])
				throw LEDError("Duplicated group map for " + elementAttr["target"] + " map " + elementAttr["trigger"]);

			itemsMap.emplace(elementAttr["trigger"], new Group::Item(
				&layout[elementAttr["target"]],
				&Color::getColor(elementAttr[PARAM_COLOR]),
				Color::str2filter(elementAttr["filter"])
			));
		}
	}
	return itemsMap;
}

string DataLoader::createFilename(const string& name) {
	return (
		string(PACKAGE_DATA_DIR)
			.append(Utility::removeChar(name, '.'))
			.append(".xml")
	);
}

void DataLoader::dropRootPrivileges(uid_t uid, gid_t gid) {

	LogDebug("Dropping privileges to user id " + to_string(uid) + " and group id " + to_string(gid));

	if (getuid() == uid) {
		LogDebug("No privileges detected");
		return;
	}

	if (not uid)
		throw LEDError("Invalid user id");

	if (not gid)
		throw LEDError("Invalid group id");

	if (setgid(gid) != 0)
		throw LEDError("Unable to change group id");

	if (setuid(uid) != 0)
		throw LEDError("Unable to change user id");

	if (chdir("/") != 0)
		throw LEDError("Unable to change directory");

	LogDebug("Privileges dropped");
}

void DataLoader::setInterval(uint8_t waitTime) {
	DataLoader::waitTime = milliseconds(waitTime);
	LogInfo("Set interval to " + to_string(DataLoader::waitTime.count()) + "ms");
}

