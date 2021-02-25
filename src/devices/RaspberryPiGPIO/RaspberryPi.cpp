/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      RaspberryPi.cpp
 * @since     Feb 15, 2020
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

#include "RaspberryPi.hpp"

using namespace LEDSpicer::Devices::RaspberryPi;

bool RaspberryPi::initialized = false;

void RaspberryPi::resetLeds() {
    for (auto & l : usedleds) {
        LogDebug("Reset Led " + to_string(l));
		gpioPWM(l, 0);
	}
	setLeds(0);
}

void RaspberryPi::openDevice() {
    usedleds.empty();
    uint8_t* firstled = getLed(0);
    for (auto& element : *getElements()) {
        LogDebug("Element " + element.second.getName());
        for (auto& pin : element.second.getPins() ) {
          uint8_t gpiopin = pin - firstled + 1;
          gpioSetMode(gpiopin, PI_OUTPUT);
          usedleds.push_back(gpiopin);
          LogDebug("gpiopin : " + to_string(gpiopin));
        }
    }

	if (initialized)
		throw Error(getFullName() + " device can only be loaded once");

	if (gpioInitialise() < 0) {
		throw Error("Failed to initialized" + getFullName());
	}

	initialized = true;
}

void RaspberryPi::closeDevice() {
	if (initialized)
		gpioTerminate();
}

string RaspberryPi::getFullName() const {
	return "Raspberry PI GPIO";
}

void RaspberryPi::drawHardwarePinMap() {
	for (uint8_t l = 0, t = LEDs.size(); l < t; ++l)
		setLed(l, l + 1);
	cout
		<< getFullName() << " Pins " << LEDs.size() << endl
		<< "Hardware pin map:" << endl
		<< " +  +" << endl << std::setfill(' ') << std::right
		<< std::setw(2) << (int)*getLed(2)  << "  +" << endl
		<< std::setw(2) << (int)*getLed(3)  << "  G" << endl
		<< std::setw(2) << (int)*getLed(4)  << std::setw(3) << (int)*getLed(14) << endl
		<< " G" << std::setw(3)                             << (int)*getLed(15) << endl
		<< std::setw(2) << (int)*getLed(17) << std::setw(3) << (int)*getLed(18) << endl
		<< std::setw(2) << (int)*getLed(27) << "  G" << endl
		<< std::setw(2) << (int)*getLed(22) << std::setw(3) << (int)*getLed(23) << endl
		<< " +" << std::setw(3)                             << (int)*getLed(24) << endl
		<< std::setw(2) << (int)*getLed(10) << "  G" << endl
		<< std::setw(2) << (int)*getLed(9)  << std::setw(3) << (int)*getLed(25) << endl
		<< std::setw(2) << (int)*getLed(11) << std::setw(3) << (int)*getLed(8)  << endl
		<< " G" << std::setw(3)                             << (int)*getLed(7)  << endl
		<< std::setw(2) << (int)*getLed(0)  << std::setw(3) << (int)*getLed(1) << endl
		<< std::setw(2) << (int)*getLed(5)  << "  G" << endl
		<< std::setw(2) << (int)*getLed(6)  << std::setw(3) << (int)*getLed(12) << endl
		<< std::setw(2) << (int)*getLed(13) << "  G" << endl
		<< std::setw(2) << (int)*getLed(19) << std::setw(3) << (int)*getLed(16) << endl
		<< std::setw(2) << (int)*getLed(26) << std::setw(3) << (int)*getLed(20) << endl
		<< " G" << std::setw(3)                             << (int)*getLed(21) << endl << endl;
}

void RaspberryPi::transfer() const {
    for (auto & l : usedleds) {
		gpioPWM(l, LEDs[l]);
    }
}
