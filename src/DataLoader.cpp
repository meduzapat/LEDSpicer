/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DataLoader.cpp
 * @since     Jun 22, 2018
 * @author    Patricio A. Rossi (MeduZa)
 * @copyright Copyright © 2018 Patricio A. Rossi (MeduZa)
 */

#include "DataLoader.hpp"

using namespace LEDSpicer;

vector<Device*> DataLoader::devices;
umap<string, Element*> DataLoader::allElements;
umap<string, Group> DataLoader::layout;
Profile* DataLoader::defaultProfile;
umap<string, Profile*> DataLoader::profiles;
string DataLoader::portNumber;

void DataLoader::readConfiguration() {

	umap<string, string> tempAttr = processNode(root);
	Utility::checkAttributes(REQUIRED_PARAM_ROOT, tempAttr, "root");

	// set log level.
	if (tempAttr.count("logLevel"))
		Log::setLogLevel(Log::str2level(tempAttr["logLevel"]));

	// set FPS.
	uint8_t fps = Utility::parseNumber(tempAttr["fps"], "Invalid value for FPS");
	if (fps == 0)
		throw LEDError("FPS = 0, No speed, nothing to do, done");
	ConnectionUSB::setInterval(1000 / (fps > 30 ? 30 : fps));
	// activate LIBUSB.
	ConnectionUSB::openSession();
	Actor::setFPS((fps > 30 ? 30 : fps));

	portNumber =  tempAttr["port"];

	processColorFile(string(PACKAGE_DATA_DIR).append("/").append(tempAttr["colors"]).append(".xml"));

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
		LogInfo("Initializing board " + device->getName() + " Id: " + std::to_string(device->getId()));
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
		umap<string, string> tempAttr = processNode(element);

		if (allElements.count(tempAttr["name"]))
			throw LEDError("Duplicated element " + tempAttr["name"]);

		Utility::checkAttributes(REQUIRED_PARAM_NAME_ONLY, tempAttr, "device element");

		// Single color.
		if (tempAttr.count("led")) {
			uint8_t pin = Utility::parseNumber(tempAttr["led"], "Invalid Value for pin in board " + device->getName()) - 1;
			device->registerElement(tempAttr["name"], pin);
			pinCheck[pin] = true;
		}
		// RGB.
		else {
			Utility::checkAttributes(REQUIRED_PARAM_RGB_LED, tempAttr, tempAttr["name"]);
			uint8_t
				r = Utility::parseNumber(tempAttr["red"], "Invalid Value for red pin in board " + device->getName()) - 1,
				g = Utility::parseNumber(tempAttr["green"], "Invalid Value for green pin in board " + device->getName()) - 1,
				b = Utility::parseNumber(tempAttr["blue"], "Invalid Value for blue pin in board " + device->getName()) - 1;
			device->registerElement(tempAttr["name"], r, g , b);
			pinCheck[r] = true;
			pinCheck[g] = true;
			pinCheck[b] = true;
		}
		allElements.emplace(tempAttr["name"], device->getElement(tempAttr["name"]));
	}

	// Checks orphan Pins.
	LogInfo(device->getName() + " with " + to_string(device->getNumberOfElements()) + " elements and " + to_string(pinCheck.size()) + " LEDs");
	for (uint8_t pin = 0; pin < pinCheck.size(); ++pin)
		if (not pinCheck[pin])
			LogNotice("Pin " + to_string(pin + 1) + " is not set for device " + device->getName());
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

		layout.emplace(tempAttr["name"], Group());
		processGroupElements(element, tempAttr["name"]);
	}

	// Create a group with all elements on it called All if not defined.
	if (not this->layout.count("All")) {
		Group group;
		for (auto& gp : allElements)
			group.linkElement(gp.second);
		group.shrinkToFit();
		layout.emplace("All", group);
	}

	defaultProfile = processProfile(defaultProfileStr);

}

void DataLoader::processGroupElements(tinyxml2::XMLElement* groupNode, const string& name) {

	tinyxml2::XMLElement* element = groupNode->FirstChildElement("element");

	if (not element)
		throw LEDError("Empty group section");

	uint8_t numPins = 0;
	umap<string, Group*> groupElements;
	for (; element; element = element->NextSiblingElement("element")) {
		umap<string, string> elementAttr = processNode(element);
		Utility::checkAttributes(REQUIRED_PARAM_NAME_ONLY, elementAttr, "group element");

		// Check dupes and add
		if (groupElements.count(elementAttr["name"]))
			throw LEDError("Duplicated element name " + elementAttr["name"] + " in group '" + name + "'");

		if (allElements.count(elementAttr["name"])) {
			layout[name].linkElement(allElements[elementAttr["name"]]);
			groupElements.emplace(elementAttr["name"], &layout[name]);
		}
		else {
			throw LEDError("Invalid element " + elementAttr["name"] + " in group '" + name + "'");
		}
	}
	layout[name].shrinkToFit();
}

Profile* DataLoader::processProfile(const string& name) {

	if (profiles.count(name))
		return profiles[name];

	XMLHelper profile(string(PACKAGE_DATA_DIR).append("/profiles/").append(name).append(".xml"), "Profile");
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
		start = createAnimation(name, tempAttr);
	}

	element = profile.getRoot()->FirstChildElement("endTransition");
	if (element) {
		umap<string, string> tempAttr = processNode(element);
		tempAttr["group"]  = "All";
		tempAttr["filter"] = "Normal";
		end = createAnimation(name, tempAttr);
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
			profilePtr->addAnimation(tempAttr["name"], processAnimation(tempAttr["name"]));
		}
	}
	profiles[name] = profilePtr;
	return profilePtr;
}

vector<Actor*> DataLoader::processAnimation(const string& name) {

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

Device* DataLoader::createDevice(umap<string, string>& deviceData) {

	uint8_t devId = deviceData.count("boardId") ? Utility::parseNumber(deviceData["boardId"], "Device id should be a number") : 1;

	if (deviceData["name"] == IPAC_ULTIMATE)
		return new UltimarcUltimate(devId, deviceData);

	if (deviceData["name"] == PAC_DRIVE)
		return new UltimarcPacDrive(devId, deviceData);


	throw LEDError("Unknown board name " + deviceData["name"]);
}

Actor* DataLoader::createAnimation(const string& name, umap<string, string>& actorData) {

	string groupName = actorData["group"];
	if (not layout.count(groupName))
		throw LEDError(groupName + " is not a valid group name in animation " + name);

	if (actorData["type"] == "Serpentine") {
		Utility::checkAttributes(REQUIRED_PARAM_ACTOR_SERPENTINE, actorData, "actor Serpentine");
		return new Serpentine(actorData, &layout.at(groupName));
	}
	if (actorData["type"] == "Pulse") {
		Utility::checkAttributes(REQUIRED_PARAM_ACTOR_PULSE, actorData, "actor Pulse");
		return new Pulse(actorData, &layout.at(groupName));
	}
	if (actorData["type"] == "Gradient") {
		Utility::checkAttributes(REQUIRED_PARAM_ACTOR_GRADIENT, actorData, "actor Gradient");
		return new Gradient(actorData, &layout.at(groupName));
	}
	if (actorData["type"] == "Random") {
		Utility::checkAttributes(REQUIRED_PARAM_ACTOR_RANDOM, actorData, "actor Random");
		return new Random(actorData, &layout.at(groupName));
	}
	if (actorData["type"] == "Filler") {
		Utility::checkAttributes(REQUIRED_PARAM_ACTOR_FILLER, actorData, "actor Filler");
		return new Filler(actorData, &layout.at(groupName));
	}
	throw LEDError("Invalid actor type '" + actorData["type"] + "' inside " + name);
}
