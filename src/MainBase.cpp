/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      MainBase.cpp
 * @since     Nov 18, 2018
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

#include "MainBase.hpp"

using namespace LEDSpicer;

bool MainBase::running = false;
Profile* MainBase::currentProfile = nullptr;
vector<Profile*> MainBase::profiles;
umap<string, Element::Item> MainBase::alwaysOnElements;
umap<string, Group::Item> MainBase::alwaysOnGroups;

MainBase::MainBase() :
	messages(
		DataLoader::getMode() == DataLoader::Modes::Normal or
		DataLoader::getMode() == DataLoader::Modes::Foreground ? DataLoader::portNumber : ""
	)
{

	profiles.push_back(DataLoader::defaultProfile);

	switch (DataLoader::getMode()) {
	case DataLoader::Modes::Dump:
	case DataLoader::Modes::Profile:
		return;
	}

	// seed the random
	std::srand(time(nullptr));

#ifndef DRY_RUN
	if (DataLoader::getMode() == DataLoader::Modes::Normal) {
		LogDebug("Daemonizing");
		if (daemon(0, 0) == -1)
			throw Error("Unable to daemonize.");
		LogDebug("Daemonized");
	}
#endif

	running = true;
}

MainBase::~MainBase() {

	for (auto& dh : DataLoader::deviceHandlers) {
		delete dh.second;
#ifdef DEVELOP
		LogDebug("Device Handler of type " + dh.first + " instance deleted");
#endif
	}

	for (auto& ah : DataLoader::actorHandlers) {
		delete ah.second;
#ifdef DEVELOP
		LogDebug("Actor Handler of type " + ah.first + " instance deleted");
#endif
	}

	for (auto i : DataLoader::inputHandlers) {
		delete i.second;
#ifdef DEVELOP
		LogDebug("Input Handler " + i.first + " instance deleted");
#endif
	}

	for (auto p : DataLoader::profiles) {
#ifdef DEVELOP
		LogDebug("Profile " + p.first + " instance deleted");
#endif
		delete p.second;
	}

	USB::closeSession();
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
			device->packData();
			continue;
		}

		if (inp == "q")
			return;

		try {
			led = std::stoi(inp) - 1;
			device->validateLed(led);
			device->setLed(led, 255);
		}
		catch (Error& e) {
			cerr << e.getMessage() << endl;
			continue;
		}
		catch (...) {
			cerr << "Invalid led number " + inp << endl;
			continue;
		}
		device->packData();
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
			device->packData();
			continue;
		}

		if (inp == "q")
			return;

		try {
			Element* element = device->getElement(inp);
			element->setColor(Color::getColor("White"));
		}
		catch (Error& e) {
			cerr << e.getMessage() << endl;
			continue;
		}
		catch (...) {
			cerr << "Invalid element " + inp << endl;
		}
		device->packData();
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
		"Interval: " << DataLoader::waitTime.count() << "ms" << endl <<
		"Total Elements registered: " << (int)DataLoader::allElements.size() << endl << endl <<
		"Layout:";
	for (auto group : DataLoader::layout) {
		cout << endl << "Group: '" << group.first << "' with ";
		group.second.drawElements();
	}
	cout << endl;
}

void MainBase::dumpProfile() {
	cout << endl << "Default Profile:" << DataLoader::defaultProfile->getName() << endl;
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
			// Deactivate any overwrite.
			alwaysOnElements.clear();
			alwaysOnGroups.clear();
			DataLoader::controlledItems.clear();
			profiles.push_back(profile);
			currentProfile = profile;
			profile->restart();
			break;
		}
		catch(Error& e) {
			LogDebug("Profile failed: " + e.getMessage());
			continue;
		}
	}
	return profile;
}

void MainBase::wait(milliseconds wasted) {
	if (wasted < DataLoader::waitTime)
		std::this_thread::sleep_for(DataLoader::waitTime - wasted);
	else
		LogWarning("Frame took longer time to render (" + to_string(wasted.count()) + "ms) that the minimal wait time (" + to_string(DataLoader::waitTime.count()) + "ms), to fix this decrease the number of FPS in the configuration");
}
