/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      MainBase.cpp
 * @since     Nov 18, 2018
 * @author    Patricio A. Rossi (MeduZa)
 * @copyright Copyright © 2018 Patricio A. Rossi (MeduZa)
 */

#include "MainBase.hpp"

using namespace LEDSpicer;

bool MainBase::running = false;

MainBase::MainBase(Modes mode) :
	messages(mode == Modes::Normal or mode == Modes::Foreground ? DataLoader::portNumber : "")
{
	profiles.push_back(DataLoader::defaultProfile);
	if (mode == Modes::Dump)
		return;

#ifndef DRY_RUN
	if (mode == Modes::Normal) {
		LogDebug("Daemonizing");
		if (daemon(0, 0) == -1) {
			throw Error("Unable to daemonize.");
		}
		LogDebug("Daemonized");
	}
	for (auto device : DataLoader::devices)
		device->initialize();
#endif

	running = true;
}

MainBase::~MainBase() {

	for (auto d : DataLoader::devices) {
		LogInfo("Closing " + d->getFullName());
		delete d;
	}

	for (auto p : DataLoader::profiles)
		delete p.second;

	ConnectionUSB::terminate();
}

void MainBase::testLeds() {
	cout << "Test LEDs (q to quit)" << endl;
	string inp;
	Device* device = selectDevice();
	if (not device)
		return;

	while (true) {
		uint8_t led;
		std::cin.clear();
		cout << endl << "Select a Led (r to reset, q to quit):" << endl;
		std::getline(std::cin, inp);

		if (inp == "r") {
			device->setLeds(0);
			device->transfer();
			continue;
		}

		if (inp == "q")
			return;

		try {
			led = std::stoi(inp) - 1;
			if (led >= device->getNumberOfLeds())
				throw "";
			device->setLed(led, 255);
			device->transfer();
		}
		catch (...) {
			cerr << "Invalid pin number" << endl;
		}
	}
}

void MainBase::testElements() {
	cout << "Test Elements (q to quit)" << endl;
	string inp;
	Device* device = selectDevice();
	if (not device)
		return;

	while (true) {
		std::cin.clear();

		for (auto e : *device->getElements())
			cout << e.second.getName() << endl;
		cout << endl << "Enter an element name (r to reset, q to quit):" << endl;

		std::getline(std::cin, inp);

		if (inp == "r") {
			device->setLeds(0);
			device->transfer();
			continue;
		}

		if (inp == "q")
			return;

		try {
			Element* element = device->getElement(inp);
			element->setColor(Color::getColor("White"));
			device->transfer();
		}
		catch (...) {
			cerr << "Invalid pin number" << endl;
		}
	}
}

void MainBase::dumpConfiguration() {
	cout << endl << "System Configuration:" << endl << "Colors:" << endl;
	Color::drawColors();
	cout  << endl << "Hardware:" << endl;
	for (auto d : DataLoader::devices)
		d->drawHardwarePinMap();
	cout <<
		"Log level: " << Log::level2str(Log::getLogLevel()) << endl <<
		"Interval: " << (int)ConnectionUSB::getInterval() << "ms" << endl <<
		"Total Elements registered: " << (int)DataLoader::allElements.size() << endl << endl <<
		"Layout:";
	for (auto group : DataLoader::layout) {
		cout << endl << "Group: '" << group.first << "' with ";
		group.second.drawElements();
	}
	cout << endl << "Default Profile:" << endl;
	DataLoader::defaultProfile->drawConfig();
	cout << endl;
}

Device* MainBase::selectDevice() {

	if (DataLoader::devices.size() == 1)
		return DataLoader::devices[0];

	string inp;
	while (true) {
		std::cin.clear();
		uint8_t deviceIndex;
		cout << "Select a device:" << endl;
		for (uint8_t c = 0; c < DataLoader::devices.size(); ++c)
			cout << c + 1 << ": " << DataLoader::devices[c]->getFullName() << endl;
		std::getline(std::cin, inp);
		if (inp == "q")
			return nullptr;
		try {
			deviceIndex = std::stoi(inp);
			if (deviceIndex > DataLoader::devices.size() or deviceIndex < 1)
				throw "";
			return DataLoader::devices[deviceIndex - 1];
			break;
		}
		catch (...) {
			cerr << "Invalid device number" << endl;
		}
	}
}

Profile* MainBase::tryProfiles(const vector<string>& data) {
	Profile* profile = nullptr;
	for (auto& profileName : data) {
		LogInfo("changing profile to " + profileName);
		try {
			profile = DataLoader::processProfile(profileName);
			profiles.push_back(profile);
			profile->reset();
			// Set Always on elements for profile.
			alwaysOnElements.clear();
			for (auto& e : profile->getElementsOverwrite())
				alwaysOnElements.emplace(e.first, ElementItem{
					DataLoader::allElements[e.first],
					e.second,
					Color::Filters::Normal
				});
			// Set Always on groups for profile.
			alwaysOnGroups.clear();
			for (auto& e : profile->getGroupsOverwrite())
				alwaysOnGroups.emplace(e.first, GroupItem{
					&DataLoader::layout[e.first],
					e.second,
					Color::Filters::Normal
				});
			break;
		}
		catch(Error& e) {
			LogDebug("Profile failed: " + e.getMessage());
			continue;
		}
	}
	return profile;
}
