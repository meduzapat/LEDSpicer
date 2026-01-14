/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      MainBase.cpp
 * @since     Nov 18, 2018
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2018 - 2026 Patricio A. Rossi (MeduZa)
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
	default: break;
	}

#ifndef DRY_RUN
	if (DataLoader::getMode() == DataLoader::Modes::Normal) {
		LogDebug("Daemonizing");
		if (daemon(0, 0) == -1)
			throw Error("Unable to daemonize");
		LogDebug("Daemonized");
	}
#endif

	running = true;
}

MainBase::~MainBase() {

	for (auto& dh : DeviceHandler::deviceHandlers) {
		delete dh.second;
#ifdef DEVELOP
		LogDebug("Device Handler of type " + dh.first + " instance deleted");
#endif
	}

	DataLoader::destroyCache();

	USB::closeSession();
}

void MainBase::testLeds() {
	cout << "Test LEDs (q to quit)" << endl;
	string inp;
	Device* device = selectDevice();
	if (not device) return;

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

		if (inp == "q") return;

		try {
			led = std::stoi(inp) - 1;
#ifndef DEVELOP
			device->validateLed(led);
#endif
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
	if (not device) return;

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

		if (inp == "q") return;

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
	for (auto d : Device::devices)
		d->drawHardwareLedMap();
	cout <<
		"Log level: " << Log::level2str(Log::getLogLevel()) << endl <<
		"Interval: " << DataLoader::waitTime.count() << "ms" << endl <<
		"Total Elements registered: " << static_cast<uint16_t>(Element::allElements.size()) << endl << endl <<
		"Layout:";
	for (auto group : Group::layout) {
		cout << endl << "Group: '" << group.first << "' with ";
		group.second.drawElements();
	}
	cout << endl << "Groups:" << endl;
	for (auto group : Group::layout)
		cout << std::setfill(' ') << std::setw(20) << std::left << group.first << "default Color: " << group.second.getDefaultColor().getName() << endl;

	cout << endl << "Elements:" << endl;
	for (auto element : Element::allElements)
		element.second->draw();

	cout << endl;
}

void MainBase::dumpProfile() {
	cout << endl << "Default Profile:" << Profile::defaultProfile->getName() << endl;
	Profile::defaultProfile->drawConfig();
	DataLoader::getTransitionFromCache(Profile::defaultProfile)->drawConfig();
	cout << endl;
}

Device* MainBase::selectDevice() {

	if (Device::devices.size() == 1)
		return Device::devices[0];

	string inp;
	while (true) {
		std::cin.clear();
		uint8_t deviceIndex;
		cout << "Select a device:" << endl;
		for (uint8_t c = 0; c < Device::devices.size(); ++c)
			cout << c + 1 << ": " << Device::devices[c]->getFullName() << endl;
		std::getline(std::cin, inp);
		if (inp == "q")
			return nullptr;
		try {
			deviceIndex = std::stoi(inp);
			if (deviceIndex > Device::devices.size() or deviceIndex < 1)
				throw "";
			return Device::devices[deviceIndex - 1];
			break;
		}
		catch (...) {
			cerr << "Invalid device number" << endl;
		}
	}
}

void MainBase::wait(milliseconds wasted) {
	if (wasted <= DataLoader::waitTime) {
		start = high_resolution_clock::now();
		sleep_for(DataLoader::waitTime - wasted);
#ifdef BENCHMARK
		LogDebug("Waited time: " + to_string(duration_cast<milliseconds>(high_resolution_clock::now() - start).count()) + "ms");
#endif
	}
	else {
		LogInfo("The frame took " + to_string(wasted.count()) + "ms to render, that is longer than the minimal wait time of " + to_string(DataLoader::waitTime.count()) + "ms.");
	}
#ifdef BENCHMARK
	LogDebug("Message time: " + to_string(timeMessage.count()) + "ms, Animation Time: " + to_string(timeAnimation.count()) + "ms, Transmission time: " + to_string(timeTransfer.count()) + "ms.");
#endif
}

void MainBase::sendData() {
	// Send data.
	// TODO: need to test speed: single thread or running one thread per device.
#ifdef BENCHMARK
	startTransfer = high_resolution_clock::now();
#endif
	for (auto device : Device::devices)
		device->packData();
#ifdef BENCHMARK
	timeTransfer = duration_cast<milliseconds>(high_resolution_clock::now() - startTransfer);
#endif
	// Wait...
	wait(duration_cast<milliseconds>(high_resolution_clock::now() - start));
}
