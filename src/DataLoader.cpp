/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DataLoader.cpp
 * @since     Jun 22, 2018
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2018 - 2025 Patricio A. Rossi (MeduZa)
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

ProfilePtrUMap    DataLoader::profilesCache;
InputPtrUMap      DataLoader::inputCache;
string            DataLoader::portNumber;
milliseconds      DataLoader::waitTime;
DataLoader::Modes DataLoader::mode = DataLoader::Modes::Normal;
ActorPtrsUmap     DataLoader::animationCache;
unordered_map<Profile*, Transition*> DataLoader::transitions;

DataLoader::Modes DataLoader::getMode() {
	return mode;
}

void DataLoader::setMode(Modes mode) {
	DataLoader::mode = mode;
}

void DataLoader::readConfiguration(const string& testProfile) {

	// Process Root.
	StringUMap tempAttr = processNode(root);
	Utility::checkAttributes(REQUIRED_PARAM_ROOT, tempAttr, "root");

	// Set log level.
	if (tempAttr.exists(PARAM_LOG_LEVEL)) {
		Log::setLogLevel(Log::str2level(tempAttr[PARAM_LOG_LEVEL]));
	}

	// Set FPS.
	uint8_t fps = Utility::parseNumber(tempAttr[PARAM_FPS], invalidValueFor("FPS"));
	if (fps == 0) throw Utilities::Error("FPS = 0, No speed, nothing to do, done");
	setInterval(1000 / (fps > MAXIMUM_FPS ? MAXIMUM_FPS : fps));
	Actor::setFPS((fps > MAXIMUM_FPS ? MAXIMUM_FPS : fps));

	// Set Port number.
	portNumber = tempAttr[PARAM_PORT];

	// Read Colors.
	processColorFile(createFilename("/" + tempAttr[PARAM_COLORS]));
	auto cs = Utility::explode(tempAttr.exists(PARAM_RANDOM_COLORS) ? tempAttr[PARAM_RANDOM_COLORS] : "", ',');
	for (auto& c : cs) Utility::trim(c);
	Color::setRandomColors(cs);

	processDevices();

	string defaultProfile(processLayout());

	// Initialize Devices.
	if (mode != Modes::Dump and mode != Modes::Profile) {
		for (auto device : Device::devices) device->initialize();
	}

	// Drop root.
	dropRootPrivileges(
		static_cast<uid_t>(Utility::parseNumber(tempAttr[PARAM_USER_ID], invalidValueFor("user ID"))),
		static_cast<gid_t>(Utility::parseNumber(tempAttr[PARAM_GROUP_ID], invalidValueFor("group ID")))
	);

	// Load the requested profile or the default.
	Profile::defaultProfile = processProfile(mode == Modes::Profile ? testProfile : defaultProfile);
	// Create ending transition at 0
	transitions.emplace(nullptr, new FadeOutIn(Profile::defaultProfile, "Normal", Color::Off));
}

void DataLoader::processColorFile(const string& file) {

	XMLHelper colors(file, "Colors");
	StringUMap colorAttr = processNode(colors.getRoot());

	tinyxml2::XMLElement* xmlElement = colors.getRoot()->FirstChildElement(NODE_COLOR);
	if (not xmlElement)
		throw Utilities::Error("No colors found");

	string format = colorAttr["format"];
	StringUMap colorsData;
	for (; xmlElement; xmlElement = xmlElement->NextSiblingElement()) {
		colorAttr = processNode(xmlElement);
		Utility::checkAttributes(REQUIRED_PARAM_COLOR, colorAttr, NODE_COLOR);
		if (colorAttr[PARAM_NAME] == "random")
			continue;
		if (colorsData.exists(colorAttr[PARAM_NAME])) {
			LogWarning("Duplicated color " + colorAttr[PARAM_NAME]);
		}
		colorsData[colorAttr[PARAM_NAME]] = colorAttr[PARAM_COLOR];
	}

	Color::loadColors(colorsData, format);
}

void DataLoader::processDevices() {

	tinyxml2::XMLElement* xmlElement = root->FirstChildElement(NODE_DEVICES);
	if (not xmlElement)
		throw Utilities::Error("Missing " NODE_DEVICES " section");

	xmlElement = xmlElement->FirstChildElement(NODE_DEVICE);
	if (not xmlElement)
		throw Utilities::Error("Empty " NODE_DEVICES " section");

	for (; xmlElement; xmlElement = xmlElement->NextSiblingElement(NODE_DEVICE)) {
		StringUMap deviceAttr = processNode(xmlElement);
		Utility::checkAttributes(REQUIRED_PARAM_DEVICE, deviceAttr, NODE_DEVICE);
		auto device = createDevice(deviceAttr);
		LogInfo("Processing " + device->getFullName());
		Device::devices.push_back(device);
		processDeviceElements(xmlElement, device);
	}
}

void DataLoader::processDeviceElements(tinyxml2::XMLElement* deviceNode, Device* device) {

	// Only used to report unused LEDs.
	vector<bool> ledCheck(device->getNumberOfLeds(), false);

	tinyxml2::XMLElement* xmlElement = deviceNode->FirstChildElement(NODE_ELEMENT);
	if (not xmlElement)
		throw Utilities::Error("Empty elements section in ") << device->getFullName();

	for (; xmlElement; xmlElement = xmlElement->NextSiblingElement(NODE_ELEMENT)) {
		StringUMap tempAttr = processNode(xmlElement);

		// Check mandatory.
		Utility::checkAttributes(REQUIRED_PARAM_NAME_ONLY, tempAttr, "device element");

		// Check dupe.
		string name(tempAttr[PARAM_NAME]);
		if (Element::allElements.exists(name))
			throw Utilities::Error("Duplicated element [" + name + "]");

		// % of brightness.
		int brightness(tempAttr.exists(PARAM_BRIGHTNESS) ?
			Utility::parseNumber(
				tempAttr[PARAM_BRIGHTNESS],
				invalidValueFor(PARAM_BRIGHTNESS) " in " + device->getFullName() + " element " + name
			) : 0);

		if (not Utility::verifyValue<int>(brightness, 1, 99, false)) brightness = 0;

		// Reference to the stored color.
		const Color& defaultColor(tempAttr.exists(PARAM_DEFAULT_COLOR) ? Color::getColor(tempAttr[PARAM_DEFAULT_COLOR]) : Color::getColor(DEFAULT_COLOR));

		// Single color.
		if (tempAttr.exists(PARAM_LED)) {
			uint16_t led(Utility::parseNumber(tempAttr[PARAM_LED], invalidValueFor(PARAM_LED) " in " + device->getFullName()) - 1);

			device->registerElement(
				name,
				led,
				defaultColor,
				0,
				brightness
			);
			ledCheck[led] = true;
		}
		// Solenoids, Motors, Recoils, any other time sensitive hardware.
		else if (tempAttr.exists(PARAM_TIMED)) {
			uint16_t solenid(Utility::parseNumber(tempAttr[PARAM_TIMED], invalidValueFor(PARAM_TIMED) " in " + device->getFullName()) - 1);
			device->registerElement(
				name,
				solenid,
				defaultColor != Color::Off ? Color::On : Color::Off,
				tempAttr.exists(PARAM_TIME_ON) ? Utility::parseNumber(tempAttr[PARAM_TIME_ON], invalidValueFor(PARAM_TIME_ON)) : DEFAULT_SOLENOID,
				0
			);
			ledCheck[solenid] = true;
		}
		// LED Strips and RGB.
		else if (tempAttr.exists(PARAM_POSITION)) {
			uint16_t
				r, g, b,
				// size is the number of PINs, If size is 3 (1x3) is just a RGB element, strips otherwise.
				size (tempAttr.exists(PARAM_STRIP) ? Utility::parseNumber(tempAttr[PARAM_STRIP], invalidValueFor(PARAM_STRIP) " in " + device->getFullName())  * 3: 3),
				// 1st RGB element
				pos  ((Utility::parseNumber(tempAttr[PARAM_POSITION], invalidValueFor(PARAM_POSITION) " in " + device->getFullName()) - 1) * 3),
				elem (1);
			string
				order(tempAttr.exists(PARAM_COLORFORMAT) ? tempAttr.at(PARAM_COLORFORMAT) : DEFAULT_ORDER),
				groupName(name);
			// Create a group for the strip only.
			if (size > 3)
				Group::layout.emplace(
					groupName,
					Group{
						groupName,
						defaultColor
					}
				);
			// Process element(s)
			for (uint16_t c = pos; c < pos + size; ++elem) {
				string elemName(name + (size > 3 ? to_string(elem) : ""));
				for (char col : order) {
					switch (col) {
					case 'r':
						ledCheck[c] = true;
						r = c++;
						break;
					case 'g':
						ledCheck[c] = true;
						g = c++;
					break;
					case 'b':
						ledCheck[c] = true;
						b = c++;
					break;
					}
				}
				device->registerElement(
					elemName,
					r, g , b,
					defaultColor,
					brightness
				);
				// Add element to group for strip only.
				if (size > 3) {
					Group::layout.at(groupName).linkElement(device->getElement(elemName));
					Element::allElements.emplace(elemName, device->getElement(elemName));
				}
			}
		}
		// Multiple RGB LEDs together as a single element.
		else if (tempAttr.exists(PARAM_POSITIONS)) {

			string order(tempAttr.exists(PARAM_COLORFORMAT) ? tempAttr.at(PARAM_COLORFORMAT) : DEFAULT_ORDER);
			vector<uint16_t> ledPositions;
			// Process leds.
			for (auto& posStr : Utility::explode(tempAttr.at(PARAM_POSITIONS), ',')) {
				uint16_t
					pos = (Utility::parseNumber(posStr, invalidValueFor(PARAM_POSITIONS) " in " + device->getFullName()) - 1),
					c   = pos * 3,
					r, g, b;
				if (not Utility::verifyValue<uint16_t>(pos, 0, device->getNumberOfElements())) {
					throw Utilities::Error("Invalid value " + posStr + " in " + tempAttr.at(PARAM_POSITIONS) + " for element " + name + " in " + device->getFullName());
				}
				// Process element(s)
				for (char col : order) {
					switch (col) {
					case 'r':
						ledCheck[c] = true;
						r = c++;
						break;
					case 'g':
						ledCheck[c] = true;
						g = c++;
					break;
					case 'b':
						ledCheck[c] = true;
						b = c++;
					break;
					}
				}
				ledPositions.push_back(r);
				ledPositions.push_back(g);
				ledPositions.push_back(b);
			}
			device->registerElement(
				name,
				ledPositions,
				defaultColor,
				brightness
			);
		}
		// RGB.
		else {
			Utility::checkAttributes(REQUIRED_PARAM_RGB_LED, tempAttr, name);
			uint16_t
				r = Utility::parseNumber(tempAttr[PARAM_RED],   invalidValueFor("red led")   " in " + device->getFullName()) - 1,
				g = Utility::parseNumber(tempAttr[PARAM_GREEN], invalidValueFor("green led") " in " + device->getFullName()) - 1,
				b = Utility::parseNumber(tempAttr[PARAM_BLUE],  invalidValueFor("blue led")  " in " + device->getFullName()) - 1;
			device->registerElement(
				name,
				r, g , b,
				defaultColor,
				brightness
			);
			ledCheck[r] = true;
			ledCheck[g] = true;
			ledCheck[b] = true;
		}
		if (not tempAttr.exists(PARAM_STRIP) )
			Element::allElements.emplace(name, device->getElement(name));
	}

	LogNotice(
		device->getFullName()                    + " with "              +
		to_string(ledCheck.size())               + " LEDs divided into " +
		to_string(device->getNumberOfElements()) + " elements"
	);
	// Checks orphan LEDs.
	for (uint16_t led = 0; led < ledCheck.size(); ++led)
		if (not ledCheck[led])
			LogInfo("LED " + to_string(led + 1) + " is not set for " + device->getFullName());
}

const string DataLoader::processLayout() {

	StringUMap tempAttr;

	tinyxml2::XMLElement* layoutNode = root->FirstChildElement(NODE_LAYOUT);
	if (not layoutNode)
		throw Utilities::Error("Missing " NODE_LAYOUT " section");

	tempAttr = processNode(layoutNode);
	Utility::checkAttributes(REQUIRED_PARAM_LAYOUT, tempAttr, NODE_LAYOUT);
	string defaultProfile(tempAttr[PARAM_DEFAULT_PROFILE]);

	tinyxml2::XMLElement* xmlElement = layoutNode->FirstChildElement(NODE_GROUP);
	if (xmlElement)
		for (; xmlElement; xmlElement = xmlElement->NextSiblingElement(NODE_GROUP)) {
			tempAttr = processNode(xmlElement);
			Utility::checkAttributes(REQUIRED_PARAM_NAME_ONLY, tempAttr, NODE_GROUP);

			if (Group::layout.exists(tempAttr[PARAM_NAME]))
				throw Utilities::Error("Duplicated group " + tempAttr[PARAM_NAME]);

			Group::layout.emplace(
				tempAttr[PARAM_NAME],
				Group{
					tempAttr[PARAM_NAME],
					Color::getColor(tempAttr.exists(PARAM_DEFAULT_COLOR) ? tempAttr[PARAM_DEFAULT_COLOR] : DEFAULT_COLOR)
				}
			);
			processGroupElements(xmlElement, Group::layout.at(tempAttr[PARAM_NAME]));
		}

	// Create a group with all elements on it called All if not defined.
	if (not Group::layout.exists("All")) {
		Group group("All", Color::getColor(DEFAULT_COLOR));
		for (auto& gp : Element::allElements)
			group.linkElement(gp.second);
		group.shrinkToFit();
		Group::layout.emplace("All", group);
	}
	return defaultProfile;
}

void DataLoader::processGroupElements(tinyxml2::XMLElement* groupNode, Group& group) {

	tinyxml2::XMLElement* xmlElement = groupNode->FirstChildElement(NODE_ELEMENT);

	if (not xmlElement)
		throw Utilities::Error("Empty group section");

	unordered_map<string, bool> groupElements;
	for (; xmlElement; xmlElement = xmlElement->NextSiblingElement(NODE_ELEMENT)) {
		StringUMap elementAttr = processNode(xmlElement);
		Utility::checkAttributes(REQUIRED_PARAM_NAME_ONLY, elementAttr, "group element");

		// Check dupes and add
		if (groupElements.exists(elementAttr[PARAM_NAME]))
			throw Utilities::Error("Duplicated element name " + elementAttr[PARAM_NAME] + " in group '" + group.getName() + "'");

		if (Element::allElements.exists(elementAttr[PARAM_NAME])) {
			group.linkElement(Element::allElements[elementAttr[PARAM_NAME]]);
			groupElements.emplace(elementAttr[PARAM_NAME], true);
		}
		else {
			throw Utilities::Error("Invalid element " + elementAttr[PARAM_NAME] + " in group '" + group.getName() + "'");
		}
	}
	group.shrinkToFit();
}

Profile* DataLoader::getProfileFromCache(const string& name) {
	if (not profilesCache.exists(name)) {
		LogDebug("Profile cache miss.");
		return nullptr;
	}

	auto profile(profilesCache.at(name));
	LogDebug("Profile from cache.");
	return profile;
}

Profile* DataLoader::processProfile(const string& name, const string& extra) {

	XMLHelper profile(createFilename(PROFILE_DIR + name), "Profile");
	StringUMap tempAttr = processNode(profile.getRoot());
	Utility::checkAttributes(REQUIRED_PARAM_PROFILE, tempAttr, "root");
	string
		backgroundColor(tempAttr.exists("backgroundColor") ? tempAttr.at("backgroundColor") : "Off"),
		transition("None");
	// Check for transition.
	tinyxml2::XMLElement* xmlElement = profile.getRoot()->FirstChildElement(NODE_TRANSITION);
	if (xmlElement) {
		tempAttr = processNode(xmlElement);
		Utility::checkAttributes(REQUIRED_PARAM_NAME_ONLY, tempAttr, "transition for profile " + name);
		transition = tempAttr[PARAM_NAME];
	}
	Settings settings(Settings::str2effect(transition), tempAttr);
	Profile* profilePtr = new Profile(
		name,
		Color::getColor(backgroundColor)
	);
	processTransition(profilePtr, settings);

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
			if (not Element::allElements.exists(tempAttr[PARAM_NAME]))
				throw Utilities::Error("Unknown element [" + tempAttr[PARAM_NAME] + "] for always on element on profile " + name);
			profilePtr->addAlwaysOnElement(
				Element::allElements.at(tempAttr[PARAM_NAME]),
				// Get element color or default color.
				tempAttr.exists(PARAM_COLOR) ? Color::getColor(tempAttr[PARAM_COLOR]) : Element::allElements.at(tempAttr[PARAM_NAME])->getDefaultColor(),
				Color::Filters::Normal
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
			if (not Group::layout.exists(tempAttr[PARAM_NAME]))
				throw Utilities::Error("Unknown group [" + tempAttr[PARAM_NAME] + "] for always on group on profile " + name);
			profilePtr->addAlwaysOnGroup(
				&Group::layout.at(tempAttr[PARAM_NAME]),
				tempAttr.exists(PARAM_COLOR) ? Color::getColor(tempAttr[PARAM_COLOR]) : Group::layout.at(tempAttr[PARAM_NAME]).getDefaultColor(),
				Color::Filters::Normal
			);
		}
	}

	// Check for input plugins.
	xmlElement = profile.getRoot()->FirstChildElement(NODE_INPUTS);
	if (xmlElement) {
		xmlElement = xmlElement->FirstChildElement(NODE_INPUT);
		if (xmlElement) {
			for (; xmlElement; xmlElement = xmlElement->NextSiblingElement(NODE_INPUT)) {
				StringUMap elementAttr = processNode(xmlElement);
				Utility::checkAttributes(REQUIRED_PARAM_NAME_ONLY, elementAttr, "Inputs");
				processInput(profilePtr, elementAttr[PARAM_NAME]);
			}
		}
	}

	// Save Cache, replace previous value if any.
	if (profilesCache.exists(name + extra)) {
		delete profilesCache[name + extra];
	}
	profilesCache[name + extra] = profilePtr;
	return profilePtr;
}

Transition* DataLoader::getTransitionFromCache(Profile* profile) {
	if (transitions.exists(profile)) return transitions.at(profile);
	return nullptr;
}

void DataLoader::processTransition(Profile* profile, const Settings& settings) {

	// Check cache
	Transition* transition = getTransitionFromCache(profile);
	if (transition) return;

	if (settings.effect == Settings::Effects::None) {
		transitions.emplace(profile, new Transition(profile));
		return;
	}

	const string speed(settings.parameters.exists("speed") ? settings.parameters.at("speed") : "Normal");

	switch (settings.effect) {
	default:
		case Settings::Effects::FadeOutIn: {
			const Color& color(settings.parameters.exists("color") and Color::hasColor(settings.parameters.at("color")) ? Color::getColor(settings.parameters.at("color")) : Color::Off);
			transition = new FadeOutIn(profile, speed, color);
			break;
		}
		case Settings::Effects::Crossfade: {
			transition = new CrossFade(profile, speed);
			break;
		}
		case Settings::Effects::Curtain: {
			string color(settings.parameters.exists("color") and Color::hasColor(settings.parameters.at("color")) ? settings.parameters.at("color") : "Off");
			StringUMap actorSettings{
				{"type",      "Filler"},
				{"group",     "All"},
				{"color",     color},
				{"speed",     speed},
				{"filter",    "Normal"},
				{"direction", "Forward"},
				{"bouncer",   "True"},
				{"cycles",    "1"}
			};
			Curtain::ClosingWays mode(Curtain::str2ClosingWays(settings.parameters.exists("closing") ? settings.parameters.at("closing") : "Both"));
			switch (mode) {
			case Curtain::ClosingWays::Right:
				actorSettings["direction"] = "Backward";
			// No Break;
			case Curtain::ClosingWays::Left:
				actorSettings["mode"] = "Normal";
				break;
			default:
			case Curtain::ClosingWays::Both:
				actorSettings["mode"] = "Curtain";
			}
			Actor* actor = DataLoader::createAnimation(actorSettings);
			transition = new Curtain(profile, actor);
			break;
		}
	}
	if (not transition) throw Utilities::Error("Invalid Transition");
	transitions.emplace(profile, transition);
}

Device* DataLoader::createDevice(StringUMap& deviceData) {

	string deviceName = deviceData["name"];
	if (not DeviceHandler::deviceHandlers.exists(deviceName))
		DeviceHandler::deviceHandlers.emplace(deviceName, new DeviceHandler(deviceName));
	return DeviceHandler::deviceHandlers[deviceName]->createDevice(deviceData);
}

vector<Actor*> DataLoader::processAnimation(const string& file, const string& extra) {

	string name(file + extra);
	// Check cache.
	if (not (Utility::globalFlags & FLAG_FORCE_RELOAD) and animationCache.exists(name)) {
		LogDebug("Animation from cache.");
		return animationCache.at(name);
	}

	XMLHelper animation(createFilename(ACTOR_DIR + file), "Animation");

	StringUMap actorData;
	tinyxml2::XMLElement* element = animation.getRoot()->FirstChildElement(NODE_ACTOR);
	if (not element)
		throw Utilities::Error("No actors found");

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

Actor* DataLoader::createAnimation(StringUMap& actorData) {

	string
		groupName = actorData.exists("group") and not actorData.at("group").empty() ? actorData["group"] : "All",
		actorName = actorData["type"];

	if (not Group::layout.exists(groupName))
		throw Utilities::Error(groupName + " is not a valid group name in animation " + actorName);

	if (not ActorHandler::actorHandlers.exists(actorName))
		ActorHandler::actorHandlers.emplace(actorName, new ActorHandler(actorName));
	return ActorHandler::actorHandlers[actorName]->createActor(actorData, &Group::layout.at(groupName));
}

void DataLoader::processInput(Profile* profile, const string& file) {

	// Check cache.
	if (not (Utility::globalFlags & FLAG_FORCE_RELOAD) and inputCache.exists(file)) {
		LogDebug("Input from cache.");
		profile->addInput(inputCache.at(file));
		return;
	}

	XMLHelper inputFile(createFilename(INPUT_DIR + file), "Input");
	StringUMap inputAttr = processNode(inputFile.getRoot());
	Utility::checkAttributes(REQUIRED_PARAM_NAME_ONLY, inputAttr, file);
	string inputName = inputAttr[PARAM_NAME];
	// If plugin is not loaded, load it.
	if (not InputHandler::inputHandlers.exists(inputName))
		InputHandler::inputHandlers.emplace(inputName, new InputHandler(inputName));
	ItemPtrUMap inputMapTmp;

	if (inputName == "Credits" or inputName == "Actions" or inputName == "Impulse" or inputName == "Blinker") {
		// Multiple source inputs
		auto listenEvents(processInputSources(inputName, inputFile.getRoot()));
		inputAttr["listenEvents"] = Utility::implode(listenEvents, FIELD_SEPARATOR);
		tinyxml2::XMLElement* mapsNode = inputFile.getRoot()->FirstChildElement("maps");
		for (size_t c = 0; c < listenEvents.size(); ++c) {
			// Check for listenEvents, at this point everything is sanitized.
			for (; mapsNode; mapsNode = mapsNode->NextSiblingElement("maps")) {
				StringUMap mapsNodeAttr = processNode(mapsNode);
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
	Input* input(InputHandler::inputHandlers[inputName]->createInput(inputAttr, inputMapTmp));
	profile->addInput(input);

	// Save Cache, replace previous value if any.
	inputCache[file] = input;
}

vector<string> DataLoader::processInputSources(const string& inputName, tinyxml2::XMLElement* inputNode) {
	// Check for listenEvents.
	tinyxml2::XMLElement* listenEventsNode = inputNode->FirstChildElement("listenEvents");
	if (not listenEventsNode) {
		throw Utilities::Error("Missing listenEvents for " + inputName);
	}
	vector<string> listenEvents;
	listenEventsNode = listenEventsNode->FirstChildElement("listenEvent");
	for (; listenEventsNode; listenEventsNode = listenEventsNode->NextSiblingElement("listenEvent")) {
		StringUMap listenEventsAttr = processNode(listenEventsNode);
		Utility::checkAttributes(REQUIRED_PARAM_NAME_ONLY, listenEventsAttr, inputName);
		listenEvents.push_back(listenEventsAttr[PARAM_NAME]);
	}
	if (listenEvents.empty())
		throw Utilities::Error("No listenEvents found for " + inputName);
	return listenEvents;
}

void DataLoader::processInputMap(tinyxml2::XMLElement* inputNode, ItemPtrUMap& inputMaps, const string& id) {

	tinyxml2::XMLElement* maps = inputNode->FirstChildElement("map");
	for (; maps; maps = maps->NextSiblingElement("map")) {
		StringUMap elementAttr = processNode(maps);
		Utility::checkAttributes(REQUIRED_PARAM_MAP, elementAttr, "map maps");

		if (elementAttr["type"] == "Element") {
			if (not Element::allElements.exists(elementAttr["target"]))
				throw Utilities::Error("Unknown Element " + elementAttr["target"] + " on map file");

			// Check dupes and add
			if (inputMaps.exists(elementAttr["trigger"]) and inputMaps[elementAttr["trigger"]]->getName() == elementAttr["target"])
				throw Utilities::Error("Duplicated element map for " + elementAttr["target"] + " map " + elementAttr["value"]);

			auto e = Element::allElements[elementAttr["target"]];
			inputMaps.emplace(id + elementAttr["trigger"], new Element::Item(
				e,
				elementAttr.exists(PARAM_COLOR) ? &Color::getColor(elementAttr[PARAM_COLOR]) : &e->getDefaultColor(),
				Color::str2filter(elementAttr.exists(PARAM_FILTER) ? elementAttr["filter"] : "Normal"),
				inputMaps.size()
			));
			continue;
		}

		if (elementAttr["type"] == "Group") {
			if (not Group::layout.exists(elementAttr["target"]))
				throw Utilities::Error("Unknown Group " + elementAttr["target"] + " on map file");

			// Check dupes and add
			if (inputMaps.exists(elementAttr["trigger"]) and inputMaps[elementAttr["trigger"]]->getName() == elementAttr["target"])
				throw Utilities::Error("Duplicated group map for " + elementAttr["target"] + " map " + elementAttr["trigger"]);

			auto& g = Group::layout.at(elementAttr["target"]);
			inputMaps.emplace(id + elementAttr["trigger"], new Group::Item(
				&g,
				elementAttr.exists(PARAM_COLOR) ? &Color::getColor(elementAttr[PARAM_COLOR]) : &g.getDefaultColor(),
				Color::str2filter(elementAttr.exists(PARAM_FILTER) ? elementAttr["filter"] : "Normal"),
				inputMaps.size()
			));
		}
	}
}

string DataLoader::createFilename(const string& name) {
	return (
		string(PROJECT_DATA_DIR)
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

	if (not uid)          throw Utilities::Error("Invalid user id");
	if (not gid)          throw Utilities::Error("Invalid group id");
	if (setgid(gid) != 0) throw Utilities::Error("Unable to change group id");
	if (setuid(uid) != 0) throw Utilities::Error("Unable to change user id");
	if (chdir("/") != 0)  throw Utilities::Error("Unable to change directory");

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
	profilesCache.clear();

	// Remove profile's transitions.
	for (auto t : transitions) delete t.second;
	transitions.clear();
#ifdef DEVELOP
		LogDebug("Transitions deleted");
#endif
}
