/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DataLoader.cpp
 * @since     Jun 22, 2018
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

#include "DataLoader.hpp"

#define invalidValueFor(val) "Invalid Value for " val

using namespace LEDSpicer;

vector<Device*> DataLoader::devices;
umap<string, Element*> DataLoader::allElements;
umap<string, Group> DataLoader::layout;
Profile* DataLoader::defaultProfile;
string DataLoader::defaultProfileName;
umap<string, Profile*> DataLoader::profilesCache;
umap<string, vector<Actor*>> DataLoader::animationCache;
umap<string, Input*> DataLoader::inputCache;
string DataLoader::portNumber;
umap<string, DeviceHandler*> DataLoader::deviceHandlers;
umap<string, ActorHandler*> DataLoader::actorHandlers;
umap<string, InputHandler*> DataLoader::inputHandlers;
DataLoader::Modes DataLoader::mode = DataLoader::Modes::Normal;
milliseconds DataLoader::waitTime;

DataLoader::Modes DataLoader::getMode() {
	return mode;
}

void DataLoader::setMode(Modes mode) {
	DataLoader::mode = mode;
}

void DataLoader::readConfiguration() {

	// Process Root.
	umap<string, string> tempAttr = processNode(root);
	Utility::checkAttributes(REQUIRED_PARAM_ROOT, tempAttr, "root");

	// Set log level.
	if (mode == Modes::Dump or mode == Modes::Profile) {
		Device::setDumpMode();
	}
	else if (tempAttr.count(PARAM_LOG_LEVEL)) {
		Log::setLogLevel(Log::str2level(tempAttr[PARAM_LOG_LEVEL]));
	}

	// Set FPS.
	uint8_t fps = Utility::parseNumber(tempAttr[PARAM_FPS], invalidValueFor("FPS"));
	if (fps == 0)
		throw LEDError("FPS = 0, No speed, nothing to do, done");
	setInterval(1000 / (fps > MAXIMUM_FPS ? MAXIMUM_FPS : fps));
	Actor::setFPS((fps > MAXIMUM_FPS ? MAXIMUM_FPS : fps));

	// Set Port number.
	portNumber = tempAttr[PARAM_PORT];

	// Read Colors.
	processColorFile(createFilename(tempAttr[PARAM_COLORS]));
	auto cs = Utility::explode(tempAttr.count(PARAM_RANDOM_COLORS) ? tempAttr[PARAM_RANDOM_COLORS] : "", ',');
	for (auto& c : cs)
		Utility::trim(c);
	Color::setRandomColors(cs);

	processDevices();

	processLayout();

	// Initialize Devices.
	for (auto device : devices)
		device->initialize();

	// Drop root.
	dropRootPrivileges(
		static_cast<uid_t>(Utility::parseNumber(tempAttr[PARAM_USER_ID], invalidValueFor("user ID"))),
		static_cast<gid_t>(Utility::parseNumber(tempAttr[PARAM_GROUP_ID], invalidValueFor("group ID")))
	);
}

void DataLoader::processColorFile(const string& file) {

	XMLHelper colors(file, "Colors");
	umap<string, string> colorAttr = processNode(colors.getRoot());

	tinyxml2::XMLElement* xmlElement = colors.getRoot()->FirstChildElement(NODE_COLOR);
	if (not xmlElement)
		throw LEDError("No colors found");

	string format = colorAttr["format"];
	umap<string, string> colorsData;
	for (; xmlElement; xmlElement = xmlElement->NextSiblingElement()) {
		colorAttr = processNode(xmlElement);
		Utility::checkAttributes(REQUIRED_PARAM_COLOR, colorAttr, NODE_COLOR);
		if (colorAttr[PARAM_NAME] == "random")
			continue;
		if (colorsData.count(colorAttr[PARAM_NAME])) {
			LogWarning("Duplicated color " + colorAttr[PARAM_NAME]);
		}
		colorsData[colorAttr[PARAM_NAME]] = colorAttr[PARAM_COLOR];
	}

	Color::loadColors(colorsData, format);
}

void DataLoader::processDevices() {

	tinyxml2::XMLElement* xmlElement = root->FirstChildElement(NODE_DEVICES);
	if (not xmlElement)
		throw LEDError("Missing " NODE_DEVICES " section");

	xmlElement = xmlElement->FirstChildElement(NODE_DEVICE);
	if (not xmlElement)
		throw LEDError("Empty " NODE_DEVICES " section");

	for (; xmlElement; xmlElement = xmlElement->NextSiblingElement(NODE_DEVICE)) {
		umap<string, string> deviceAttr = processNode(xmlElement);
		Utility::checkAttributes(REQUIRED_PARAM_DEVICE, deviceAttr, NODE_DEVICE);
		auto device = createDevice(deviceAttr);
		LogInfo("Processing " + device->getFullName());
		this->devices.push_back(device);
		processDeviceElements(xmlElement, device);
	}
}

void DataLoader::processDeviceElements(tinyxml2::XMLElement* deviceNode, Device* device) {

	// Only used to report unused pins.
	vector<bool> pinCheck(device->getNumberOfLeds(), false);

	tinyxml2::XMLElement* xmlElement = deviceNode->FirstChildElement(NODE_ELEMENT);
	if (not xmlElement)
		throw LEDError("Empty elements section in " + device->getFullName());

	for (; xmlElement; xmlElement = xmlElement->NextSiblingElement(NODE_ELEMENT)) {
		umap<string, string> tempAttr = processNode(xmlElement);

		if (allElements.count(tempAttr[PARAM_NAME]))
			throw LEDError("Duplicated element " + tempAttr[PARAM_NAME]);

		Utility::checkAttributes(REQUIRED_PARAM_NAME_ONLY, tempAttr, "device element");

		int brightness = tempAttr.count(PARAM_BRIGHTNESS) ?
			Utility::parseNumber(
				tempAttr[PARAM_BRIGHTNESS],
				invalidValueFor(PARAM_BRIGHTNESS) " in " + device->getFullName() + " element " + tempAttr[PARAM_NAME]
			) : 0;
		if (not Utility::verifyValue(brightness, 1, 99, false)) {
			brightness = 0;
		}

		// Single color.
		if (tempAttr.count(PARAM_LED)) {
			uint8_t pin = Utility::parseNumber(tempAttr[PARAM_LED], invalidValueFor(PARAM_LED) " in " + device->getFullName()) - 1;

			device->registerElement(
				tempAttr[PARAM_NAME],
				pin,
				tempAttr.count(PARAM_DEFAULT_COLOR) ? Color::getColor(tempAttr[PARAM_DEFAULT_COLOR]) : Color::getColor(DEFAULT_COLOR),
				0,
				brightness
			);
			pinCheck[pin] = true;
		}
		// Solenoids, Motors, Recoils, any other time sensitive hardware.
		else if (tempAttr.count(PARAM_TIMED)) {
			uint8_t pin = Utility::parseNumber(tempAttr[PARAM_TIMED], invalidValueFor(PARAM_TIMED) " in " + device->getFullName()) - 1;
			device->registerElement(
				tempAttr[PARAM_NAME],
				pin,
				tempAttr.count(PARAM_DEFAULT_COLOR) ? Color::getColor(tempAttr[PARAM_DEFAULT_COLOR]) : Color::getColor("On"),
				tempAttr.count(PARAM_TIME_ON) ? Utility::parseNumber(tempAttr[PARAM_TIME_ON], invalidValueFor(PARAM_TIME_ON)) : DEFAULT_SOLENOID,
				0
			);
			pinCheck[pin] = true;
		}
		// RGB.
		else {
			Utility::checkAttributes(REQUIRED_PARAM_RGB_LED, tempAttr, tempAttr[PARAM_NAME]);
			uint8_t
				r = Utility::parseNumber(tempAttr[PARAM_RED], invalidValueFor(  "red pin")   " in " + device->getFullName()) - 1,
				g = Utility::parseNumber(tempAttr[PARAM_GREEN], invalidValueFor("green pin") " in " + device->getFullName()) - 1,
				b = Utility::parseNumber(tempAttr[PARAM_BLUE], invalidValueFor( "blue pin")  " in " + device->getFullName()) - 1;
			device->registerElement(
				tempAttr[PARAM_NAME],
				r, g , b,
				tempAttr.count(PARAM_DEFAULT_COLOR) ? Color::getColor(tempAttr[PARAM_DEFAULT_COLOR]) : Color::getColor(DEFAULT_COLOR),
				brightness
			);
			pinCheck[r] = true;
			pinCheck[g] = true;
			pinCheck[b] = true;
		}
		allElements.emplace(tempAttr[PARAM_NAME], device->getElement(tempAttr[PARAM_NAME]));
	}

	LogInfo(
		device->getFullName()                    + " with "              +
		to_string(pinCheck.size())               + " LEDs divided into " +
		to_string(device->getNumberOfElements()) + " elements"
	);
	// Checks orphan Pins.
	for (uint8_t pin = 0; pin < pinCheck.size(); ++pin)
		if (not pinCheck[pin]) {
			LogInfo("Pin " + to_string(pin + 1) + " is not set for " + device->getFullName());
		}
}

void DataLoader::processLayout() {

	umap<string, string> tempAttr;

	tinyxml2::XMLElement* layoutNode = root->FirstChildElement(NODE_LAYOUT);
	if (not layoutNode)
		throw LEDError("Missing " NODE_LAYOUT " section");

	tempAttr = processNode(layoutNode);
	Utility::checkAttributes(REQUIRED_PARAM_LAYOUT, tempAttr, NODE_LAYOUT);
	defaultProfileName = tempAttr[PARAM_DEFAULT_PROFILE];

	tinyxml2::XMLElement* xmlElement = layoutNode->FirstChildElement(NODE_GROUP);
	if (xmlElement)
		for (; xmlElement; xmlElement = xmlElement->NextSiblingElement(NODE_GROUP)) {
			tempAttr = processNode(xmlElement);
			Utility::checkAttributes(REQUIRED_PARAM_NAME_ONLY, tempAttr, NODE_GROUP);

			if (this->layout.count(tempAttr[PARAM_NAME]))
				throw LEDError("Duplicated group " + tempAttr[PARAM_NAME]);

			layout.emplace(
				tempAttr[PARAM_NAME],
				Group{
					tempAttr[PARAM_NAME],
					Color::getColor(tempAttr.count(PARAM_DEFAULT_COLOR) ? tempAttr[PARAM_DEFAULT_COLOR] : DEFAULT_COLOR)
				}
			);
			processGroupElements(xmlElement, layout.at(tempAttr[PARAM_NAME]));
		}

	// Create a group with all elements on it called All if not defined.
	if (not this->layout.count("All")) {
		Group group("All", Color::getColor(DEFAULT_COLOR));
		for (auto& gp : allElements)
			group.linkElement(gp.second);
		group.shrinkToFit();
		layout.emplace("All", group);
	}
}

void DataLoader::processGroupElements(tinyxml2::XMLElement* groupNode, Group& group) {

	tinyxml2::XMLElement* xmlElement = groupNode->FirstChildElement(NODE_ELEMENT);

	if (not xmlElement)
		throw LEDError("Empty group section");

	umap<string, bool> groupElements;
	for (; xmlElement; xmlElement = xmlElement->NextSiblingElement(NODE_ELEMENT)) {
		umap<string, string> elementAttr = processNode(xmlElement);
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

Profile* DataLoader::getProfileFromCache(const string& name) {
	if (not profilesCache.count(name))
		return nullptr;

	auto profile(profilesCache.at(name));
	if (not (Utility::globalFlags & FLAG_FORCE_RELOAD)) {
		LogDebug("Profile from cache.");
		return profile;
	}
	LogDebug("Ignoring cache, reloading profile.");
	delete profile;
	return nullptr;
}

Profile* DataLoader::processProfile(const string& name, const string& extra) {

	XMLHelper profile(createFilename(PROFILE_DIR + name), "Profile");
	umap<string, string> tempAttr = processNode(profile.getRoot());
	Utility::checkAttributes(REQUIRED_PARAM_PROFILE, tempAttr, "root");

	vector<Actor*>
		startTransitions,
		endTransitions;

	int startTransitionElementTime = 0;
	tinyxml2::XMLElement* xmlElement = profile.getRoot()->FirstChildElement(NODE_START_TRANSITIONS);
	if (xmlElement) {
		// Check for show element timer.
		tempAttr = processNode(xmlElement);
		if (tempAttr.count("showElementTimer")) {
			startTransitionElementTime = Utility::parseNumber(tempAttr.at("showElementTimer"), "Invalid number of ms for start transition showElementTimer");
		}
		// Check for animations.
		xmlElement = xmlElement->FirstChildElement(NODE_ANIMATION);
		for (; xmlElement; xmlElement = xmlElement->NextSiblingElement(NODE_ANIMATION)) {
			tempAttr = processNode(xmlElement);
			Utility::checkAttributes(REQUIRED_PARAM_NAME_ONLY, tempAttr, "animations for " NODE_START_TRANSITIONS " inside profile " + name);
			for (auto a : processAnimation(tempAttr[PARAM_NAME], NODE_START_TRANSITIONS)) {
				if (not tempAttr.count("cycles") and not tempAttr.count("endTime")) {
					if (a->acceptCycles())
						a->setEndCycles(DEFAULT_ENDCYCLES);
					else
						a->setEndTime(DEFAULT_ENDTIME);
				}
				startTransitions.push_back(a);
			}
		}
	}

	int endTransitionElementTime = 0;
	xmlElement = profile.getRoot()->FirstChildElement(NODE_END_TRANSITIONS);
	if (xmlElement) {
		// Check for show element timer.
		tempAttr = processNode(xmlElement);
		if (tempAttr.count("hideElementTimer")) {
			endTransitionElementTime = Utility::parseNumber(tempAttr.at("hideElementTimer"), "Invalid number of ms for end transition hideElementTimer");
		}
		// Check for animations.
		xmlElement = xmlElement->FirstChildElement(NODE_ANIMATION);
		for (; xmlElement; xmlElement = xmlElement->NextSiblingElement(NODE_ANIMATION)) {
			tempAttr = processNode(xmlElement);
			Utility::checkAttributes(REQUIRED_PARAM_NAME_ONLY, tempAttr, "animations for " NODE_END_TRANSITIONS " inside profile " + name);
			for (auto a : processAnimation(tempAttr[PARAM_NAME], NODE_START_TRANSITIONS)) {
				if (not tempAttr.count("cycles") and not tempAttr.count("endTime")) {
					if (a->acceptCycles())
						a->setEndCycles(DEFAULT_ENDCYCLES);
					else
						a->setEndTime(DEFAULT_ENDTIME);
				}
				endTransitions.push_back(a);
			}
		}
	}

	Profile* profilePtr = new Profile(
		name,
		Color(tempAttr["backgroundColor"]),
		startTransitions,
		endTransitions,
		milliseconds(startTransitionElementTime),
		milliseconds(endTransitionElementTime)
	);

	// Check for animations.
	xmlElement = profile.getRoot()->FirstChildElement(NODE_ANIMATIONS);
	if (xmlElement) {
		xmlElement = xmlElement->FirstChildElement(NODE_ANIMATION);
		for (; xmlElement; xmlElement = xmlElement->NextSiblingElement(NODE_ANIMATION)) {
			tempAttr = processNode(xmlElement);
			Utility::checkAttributes(REQUIRED_PARAM_NAME_ONLY, tempAttr, "animations for profile " + name);
			profilePtr->addAnimation(processAnimation(tempAttr[PARAM_NAME]));
		}
	}

	// Check for always on elements.
	xmlElement = profile.getRoot()->FirstChildElement("alwaysOnElements");
	if (xmlElement) {
		xmlElement = xmlElement->FirstChildElement(NODE_ELEMENT);
		for (; xmlElement; xmlElement = xmlElement->NextSiblingElement(NODE_ELEMENT)) {
			tempAttr = processNode(xmlElement);
			Utility::checkAttributes(REQUIRED_PARAM_NAME_ONLY, tempAttr, "alwaysOnElements for profile " + name);
			if (not allElements.count(tempAttr[PARAM_NAME]))
				throw LEDError("Unknown element [" + tempAttr[PARAM_NAME] + "] for always on element on profile " + name);
			profilePtr->addAlwaysOnElement(
				allElements.at(tempAttr[PARAM_NAME]),
				tempAttr.count(PARAM_COLOR) ? Color::getColor(tempAttr[PARAM_COLOR]) : allElements.at(tempAttr[PARAM_NAME])->getDefaultColor()
			);
		}
	}

	// Check for always on groups.
	xmlElement = profile.getRoot()->FirstChildElement("alwaysOnGroups");
	if (xmlElement) {
		xmlElement = xmlElement->FirstChildElement(NODE_GROUP);
		for (; xmlElement; xmlElement = xmlElement->NextSiblingElement(NODE_GROUP)) {
			tempAttr = processNode(xmlElement);
			Utility::checkAttributes(REQUIRED_PARAM_NAME_ONLY, tempAttr, "alwaysOnGroups for profile " + name);
			if (not layout.count(tempAttr[PARAM_NAME]))
				throw LEDError("Unknown group [" + tempAttr[PARAM_NAME] + "] for always on group on profile " + name);
			profilePtr->addAlwaysOnGroup(
				&layout.at(tempAttr[PARAM_NAME]),
				tempAttr.count(PARAM_COLOR) ? Color::getColor(tempAttr[PARAM_COLOR]) : layout.at(tempAttr[PARAM_NAME]).getDefaultColor()
			 );
		}
	}

	// Check for input plugins.
	xmlElement = profile.getRoot()->FirstChildElement(NODE_INPUTS);
	if (xmlElement) {
		xmlElement = xmlElement->FirstChildElement(NODE_INPUT);
		if (xmlElement) {
			for (; xmlElement; xmlElement = xmlElement->NextSiblingElement(NODE_INPUT)) {
				umap<string, string> elementAttr = processNode(xmlElement);
				Utility::checkAttributes(REQUIRED_PARAM_NAME_ONLY, elementAttr, "Inputs");
				processInput(profilePtr, elementAttr[PARAM_NAME]);
			}
		}
	}

	// Save Cache, replace previous value if any.
	profilesCache[name + extra] = profilePtr;
	return profilePtr;
}

Device* DataLoader::createDevice(umap<string, string>& deviceData) {

	string deviceName = deviceData["name"];
	if (not deviceHandlers.count(deviceName))
		deviceHandlers.emplace(deviceName, new DeviceHandler(deviceName));
	return deviceHandlers[deviceName]->createDevice(deviceData);
}

vector<Actor*> DataLoader::processAnimation(const string& file, const string& extra) {

	string name(file + extra);
	// Check cache.
	if (not (Utility::globalFlags & FLAG_FORCE_RELOAD) and animationCache.count(name)) {
		LogDebug("Animation from cache.");
		return animationCache.at(name);
	}

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

	// Save Cache, replace previous value if any.
	animationCache[name] = std::move(actors);
	return animationCache.at(name);
}

Actor* DataLoader::createAnimation(umap<string, string>& actorData) {

	string
		groupName = actorData.count("group") and not actorData.at("group").empty() ? actorData["group"] : "All",
		actorName = actorData["type"];

	if (not layout.count(groupName))
		throw LEDError(groupName + " is not a valid group name in animation " + actorName);

	if (not actorHandlers.count(actorName))
		actorHandlers.emplace(actorName, new ActorHandler(actorName));
	return actorHandlers[actorName]->createActor(actorData, &layout.at(groupName));
}

void DataLoader::processInput(Profile* profile, const string& file) {

	// Check cache.
	if (not (Utility::globalFlags & FLAG_FORCE_RELOAD) and inputCache.count(file)) {
		LogDebug("Input from cache.");
		profile->addInput(inputCache.at(file));
		return;
	}

	XMLHelper inputFile(createFilename(INPUT_DIR + file), "Input");
	umap<string, string> inputAttr = processNode(inputFile.getRoot());
	Utility::checkAttributes(REQUIRED_PARAM_NAME_ONLY, inputAttr, file);
	string inputName = inputAttr[PARAM_NAME];
	// If plugin is not loaded, load it.
	if (not inputHandlers.count(inputName))
		inputHandlers.emplace(inputName, new InputHandler(inputName));
	umap<string, Items*> inputMapTmp;

	if (inputName == "Credits" or inputName == "Actions" or inputName == "Impulse" or inputName == "Blinker") {
		// Multiple source inputs
		auto listenEvents(processInputSources(inputName, inputFile.getRoot()));
		inputAttr["listenEvents"] = Utility::implode(listenEvents, FIELD_SEPARATOR);
		tinyxml2::XMLElement* mapsNode = inputFile.getRoot()->FirstChildElement("maps");
		uint count = 0;
		for (size_t c = 0; c < listenEvents.size(); ++c) {
			// Check for listenEvents, at this point everything is sanitized.
			for (; mapsNode; mapsNode = mapsNode->NextSiblingElement("maps")) {
				umap<string, string> mapsNodeAttr = processNode(mapsNode);
				Utility::checkAttributes({"source"}, mapsNodeAttr, listenEvents[c]);
				if (mapsNodeAttr["source"] == listenEvents[c]) {
					processInputMap(mapsNode, inputMapTmp, to_string(c));
					break;
				}
			}
		}
	}
	else {
		// Single source or malformed.
		processInputMap(inputFile.getRoot(), inputMapTmp);
	}
	auto input(inputHandlers[inputName]->createInput(inputAttr, inputMapTmp));
	profile->addInput(input);

	// Save Cache, replace previous value if any.
	inputCache[file] = input;
}

vector<string> DataLoader::processInputSources(const string& inputName, tinyxml2::XMLElement* inputNode) {
	// Check for listenEvents.
	tinyxml2::XMLElement* listenEventsNode = inputNode->FirstChildElement("listenEvents");
	if (not listenEventsNode) {
		throw LEDError("Missing listenEvents for " + inputName);
	}
	vector<string> listenEvents;
	listenEventsNode = listenEventsNode->FirstChildElement("listenEvent");
	for (; listenEventsNode; listenEventsNode = listenEventsNode->NextSiblingElement("listenEvent")) {
		umap<string, string> listenEventsAttr = processNode(listenEventsNode);
		Utility::checkAttributes(REQUIRED_PARAM_NAME_ONLY, listenEventsAttr, inputName);
		listenEvents.push_back(listenEventsAttr[PARAM_NAME]);
	}
	if (listenEvents.empty())
		throw LEDError("No listenEvents found for " + inputName);
	return listenEvents;
}

void DataLoader::processInputMap(tinyxml2::XMLElement* inputNode, umap<string, Items*>& inputMaps, const string& id) {

	tinyxml2::XMLElement* maps = inputNode->FirstChildElement("map");
	for (; maps; maps = maps->NextSiblingElement("map")) {
		umap<string, string> elementAttr = processNode(maps);
		Utility::checkAttributes(REQUIRED_PARAM_MAP, elementAttr, "map maps");

		if (elementAttr["type"] == "Element") {
			if (not allElements.count(elementAttr["target"]))
				throw LEDError("Unknown Element " + elementAttr["target"] + " on map file");

			// Check dupes and add
			if (inputMaps.count(elementAttr["trigger"]) and inputMaps[elementAttr["trigger"]]->getName() == elementAttr["target"])
				throw LEDError("Duplicated element map for " + elementAttr["target"] + " map " + elementAttr["value"]);

			auto e = allElements[elementAttr["target"]];
			inputMaps.emplace(id + elementAttr["trigger"], new Element::Item(
				e,
				elementAttr.count(PARAM_COLOR) ? &Color::getColor(elementAttr[PARAM_COLOR]) : &e->getDefaultColor(),
				Color::str2filter(elementAttr.count(PARAM_FILTER) ? elementAttr["filter"] : "Normal"),
				inputMaps.size()
			));
			continue;
		}

		if (elementAttr["type"] == "Group") {
			if (not layout.count(elementAttr["target"]))
				throw LEDError("Unknown Group " + elementAttr["target"] + " on map file");

			// Check dupes and add
			if (inputMaps.count(elementAttr["trigger"]) and inputMaps[elementAttr["trigger"]]->getName() == elementAttr["target"])
				throw LEDError("Duplicated group map for " + elementAttr["target"] + " map " + elementAttr["trigger"]);

			auto& g = layout.at(elementAttr["target"]);
			inputMaps.emplace(id + elementAttr["trigger"], new Group::Item(
				&g,
				elementAttr.count(PARAM_COLOR) ? &Color::getColor(elementAttr[PARAM_COLOR]) : &g.getDefaultColor(),
				Color::str2filter(elementAttr.count(PARAM_FILTER) ? elementAttr["filter"] : "Normal"),
				inputMaps.size()
			));
		}
	}
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

void DataLoader::destroyCache() {
	for (auto p : profilesCache) {
#ifdef DEVELOP
		LogDebug("Profile " + p.first + " instance deleted");
#endif
		delete p.second;
	}
}
