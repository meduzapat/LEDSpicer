/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file		Board.cpp
 * @ingroup
 * @since		Jun 7, 2018
 * @author		Patricio A. Rossi (MeduZa)
 * @copyright	Copyright © 2018 Patricio A. Rossi (MeduZa)
 */

#include "Device.hpp"

using namespace LEDSpicer::Devices;

Device* Device::setLed(uint8_t led, uint8_t intensity) {
	validateLed(led);
	LEDs[led] = intensity;
	return this;
}

Device* Device::setLeds(uint8_t intensity) {
	for (uint8_t c = 1; c < LEDs.size(); c++) {
		setLed(c, intensity);
	}
	return this;
}

uint8_t* Device::getLED(uint8_t ledPos) {
	if (not ledPos or ledPos > board.LEDs)
		throw Error("Invalid pin number " + to_string(ledPos) + " for " + board.name);
	return &LEDs.at(ledPos);
}

string Device::getName() {
	return board.name;
}

uint8_t Device::getNumberOfLeds() {
	return board.LEDs;
}

#ifdef DEVELOP
void Device::printDeviceInformation() {
	libusb_device *device = libusb_get_device(handle);
	libusb_device_descriptor descriptor;
	if (!device)
		throw Error("Unable to retrieve device information");

	uint8_t value = 0;

	if (libusb_get_device_descriptor(device, &descriptor))
		throw Error("Unable to retrieve device descriptor");

	cout << "Number of possible configurations: " << (int) descriptor.bNumConfigurations << endl;
	cout << "Device Class: " << (int) descriptor.bDeviceClass << endl;

	libusb_config_descriptor *config;

	if (libusb_get_config_descriptor(device, 0, &config))
		throw Error("Unable to retrieve device config information");

	cout << "Interfaces: " << (int) config->bNumInterfaces << endl;

	const libusb_interface *inter;
	const libusb_interface_descriptor *interdesc;
	const libusb_endpoint_descriptor *epdesc;

	for (int i = 0; i < (int) config->bNumInterfaces; i++) {

		inter = &config->interface[i];

		cout << "Number of alternate settings: " << inter->num_altsetting << endl;

		for (int j = 0; j < inter->num_altsetting; j++) {

			interdesc = &inter->altsetting[j];

			cout << "Interface Number: " << (int) interdesc->bInterfaceNumber << endl;
			cout << "Number of endpoints: " << (int) interdesc->bNumEndpoints << endl;

			for (int k = 0; k < (int) interdesc->bNumEndpoints; k++) {

				epdesc = &interdesc->endpoint[k];

				cout << "Descriptor Type: " << (int) epdesc->bDescriptorType << endl;
				cout << "EP Address: " << (int) epdesc->bEndpointAddress << endl;
			}
		}
	}
	libusb_free_config_descriptor(config);
}
#endif

void Device::registerElement(const string& name, uint8_t led) {
	validateLed(led);
	elementsByName.emplace(name, Element(name, &LEDs[led]));
}

void Device::registerElement(const string& name, uint8_t led1, uint8_t led2, uint8_t led3) {
	validateLed(led1);
	validateLed(led2);
	validateLed(led3);
	elementsByName.emplace(name, Element(name, &LEDs[led1], &LEDs[led2], &LEDs[led3]));
}

Element* Device::getElement(const string& name) {
	return &elementsByName.at(name);
}

void Device::validateLed(uint8_t led) const {
	if (not led or led >= LEDs.size())
		throw Error("Invalid led number " + to_string(led));
}

uint8_t Device::getNumberOfElements() const {
	return elementsByName.size();
}

