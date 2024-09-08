/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      MainBase.cpp
 * @since     Nov 18, 2018
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

#include "MainBase.hpp"

using namespace LEDSpicer;

bool MainBase::running = false;
Profile* MainBase::currentProfile = nullptr;
vector<Profile*> MainBase::profiles;
high_resolution_clock::time_point MainBase::start;
#ifdef BENCHMARK
high_resolution_clock::time_point MainBase::startAnimation;
high_resolution_clock::time_point MainBase::startMessage;
high_resolution_clock::time_point MainBase::startTransfer;

milliseconds MainBase::timeAnimation;
milliseconds MainBase::timeMessage;
milliseconds MainBase::timeTransfer;
#endif

MainBase::MainBase() :
	messages(
		DataLoader::getMode() == DataLoader::Modes::Normal or
		DataLoader::getMode() == DataLoader::Modes::Foreground ? DataLoader::portNumber : ""
	)
{

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

	DataLoader::destroyCache();

	USB::closeSession();
}

void MainBase::testLeds() {
	cout << "Test LEDs (q to quit)" << endl;
	string inp;
	Device* device = selectDevice();
	if (not device)
		return;

	while (true) {
		uint16_t led;
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
		d->drawHardwareLedMap();
	cout <<
		"Log level: " << Log::level2str(Log::getLogLevel()) << endl <<
		"Interval: " << DataLoader::waitTime.count() << "ms" << endl <<
		"Total Elements registered: " << static_cast<uint16_t>(DataLoader::allElements.size()) << endl << endl <<
		"Layout:";
	for (auto group : DataLoader::layout) {
		cout << endl << "Group: '" << group.first << "' with ";
		group.second.drawElements();
	}
	cout << endl << "Groups:" << endl;
	for (auto group : DataLoader::layout)
		cout << std::setfill(' ') << std::setw(20) << std::left << group.first << "default Color: " << group.second.getDefaultColor().getName() << endl;

	cout << endl << "Elements:" << endl;
	for (auto element : DataLoader::allElements)
		element.second->draw();

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

void MainBase::wait(milliseconds wasted) {
	if (wasted < DataLoader::waitTime) {
		start = high_resolution_clock::now();
		std::this_thread::sleep_for(DataLoader::waitTime - wasted);
		LogDebug("Waited time: " + to_string(duration_cast<milliseconds>(high_resolution_clock::now() - start).count()) + "ms");
	}
	else {
		LogInfo("The frame took " + to_string(wasted.count()) + "ms to render, that is longer than the minimal wait time of " + to_string(DataLoader::waitTime.count()) + "ms.");
	}
#ifdef BENCHMARK
	LogDebug("Message time: " + to_string(timeMessage.count()) + "ms, Animation Time: " + to_string(timeAnimation.count()) + "ms, Transmission time: " + to_string(timeTransfer.count()) + "ms.");
#endif
}

Profile* MainBase::tryProfiles(const vector<string>& data) {
	Profile
		* profile    = nullptr,
		* oldProfile = nullptr;
	bool
		replace(profiles.size() and (Utility::globalFlags & FLAG_REPLACE)),
		reload(Utility::globalFlags & FLAG_FORCE_RELOAD),
		isDefault(false);
	for (auto& profileName : data) {
		LogInfo((replace ? "Replacing current profile with " : "Changing to profile ") + profileName);
		try {
			// Check cache.
			oldProfile = DataLoader::getProfileFromCache(profileName);
			// Ensure that the default profile is updated if needed.
			if (oldProfile and reload) {
				isDefault = oldProfile == DataLoader::defaultProfile;
				LogDebug("Ignoring cache, reloading profile.");
			}
			// If not in cache is the same as reload.
			if (not oldProfile or reload) {
				profile = DataLoader::processProfile(profileName);
			}
			// If reload, delete old, reload is for testing, no animations.
			if (reload) {
				if (isDefault)
					DataLoader::defaultProfile = profile;
				// Update all copies before delete.
				for (auto& p : profiles) {
					if (p == oldProfile)
						p = profile;
				}
				delete oldProfile;
			}
			// If exist and no reload use cache.
			else if (oldProfile) {
				profile = oldProfile;
			}
			if (replace) {
				if (not reload)
					terminateCurrentProfile();
				profiles.pop_back();
			}
			else {
				// Deactivate any overwrite.
				clearOverrides();
				if (not reload)
					currentProfile->stop();
			}
			profiles.push_back(profile);
			currentProfile = profile;
			// A profile need to be restarted in order to work or will be rejected and replaced.
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

Profile* MainBase::craftProfile(const string& name, const string& elements, const string& groups) {

	Profile* profile = nullptr;
	bool replace(profiles.size() and (Utility::globalFlags & FLAG_REPLACE));
	string profileName(name + elements + groups);
	try {
		// Check cache.
		profile = DataLoader::getProfileFromCache(profileName);
		if (not profile) {
			LogDebug("Creating profile with " EMPTY_PROFILE + name);
			profile = DataLoader::processProfile(EMPTY_PROFILE + name, elements + groups);
			// Add elements.
			LogDebug("Adding elements");
			for (string& n : Utility::explode(elements, FIELD_SEPARATOR)) {
				const Color* col = nullptr;
				auto parts = Utility::explode(n, GROUP_SEPARATOR);
				n = parts[0];
				if (not DataLoader::allElements.count(n)) {
					LogDebug("Unknown element " + n);
					continue;
				}
				if (parts.size() == 2 and Color::hasColor(parts[1]))
					col = &Color::getColor(parts[1]);
				else
					col = &DataLoader::allElements.at(n)->getDefaultColor();

				LogDebug("Using element " + n + " color " + col->getName());
				profile->addAlwaysOnElement(DataLoader::allElements.at(n), *col, Color::Filters::Normal);
			}

			// Add Groups.
			/*
			LogDebug("Adding groups");
			for (string& n : Utility::explode(groups, FIELD_SEPARATOR)) {
				LogDebug("Using group " + n);
				if (DataLoader::layout.count(n)) {
					profile->addAlwaysOnGroup(
						&DataLoader::layout.at(n), DataLoader::allElements.at(n)->getDefaultColor()
					);
				}
				else {
					LogDebug(n + " failed");
				}
			}
			*/

			// Add Animations.
			LogDebug("Adding Animations");
			for (string& n : Utility::explode(groups, FIELD_SEPARATOR)) {
				LogDebug("Loading animation " + n);
				try {
					profile->addAnimation(DataLoader::processAnimation(n));
				}
				catch (...) {
					LogDebug(n + " failed");
					continue;
				}
			}

			// Add Inputs.
			LogDebug("Adding Inputs");
			for (string& n : Utility::explode(groups, FIELD_SEPARATOR)) {
				LogDebug("Loading input " + n);
				try {
					DataLoader::processInput(profile, n);
				}
				catch (...) {
					LogDebug(n + " failed");
					continue;
				}
			}
		}
		if (replace) {
			terminateCurrentProfile();
			profiles.pop_back();
		}
		else {
			// Deactivate any overwrite.
			clearOverrides();
			currentProfile->stop();
		}
		profiles.push_back(profile);
		currentProfile = profile;
		profile->restart();
	}
	catch (Error& e) {
		LogNotice(e.getMessage());
	}
	return profile;
}

void MainBase::terminateCurrentProfile() {
	// Deactivate forced elements and groups so the end transition is rendered without interference.
	clearOverrides();
	currentProfile->terminate();

	// Wait for termination.
	while (currentProfile->isRunning()) {
		// Reset micro-frame.
		start = high_resolution_clock::now();
		// Frame begins.
		currentProfile->runFrame();
		sendData();
	}
}

void MainBase::sendData() {
	// Send data.
	// TODO: need to test speed: single thread or running one thread per device.
#ifdef BENCHMARK
	startTransfer = high_resolution_clock::now();
#endif
	for (auto device : DataLoader::devices)
		device->packData();
#ifdef BENCHMARK
	timeTransfer = duration_cast<milliseconds>(high_resolution_clock::now() - startTransfer);
#endif
	// Wait...
	wait(duration_cast<milliseconds>(high_resolution_clock::now() - start));
}

void MainBase::clearOverrides() {
	Profile::removeTemporaryAlwaysOnGroups();
	Profile::removeTemporaryAlwaysOnElements();
	Input::clearControlledInputs();
}
