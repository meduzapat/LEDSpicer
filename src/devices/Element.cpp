/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file		Element.cpp
 * @since		Jun 22, 2018
 * @author		Patricio A. Rossi (MeduZa)
 * @copyright	Copyright © 2018 Patricio A. Rossi (MeduZa)
 */

#include "Element.hpp"

using namespace LEDSpicer::Devices;

void Element::setColor(const Color& color) {
	if (pins.size() == 3) {
		*pins[0] = color.getR();
		*pins[1] = color.getG();
		*pins[2] = color.getB();
	}
	else {
		*pins[0] = color.getMonochrome();
	}
}

void Element::setPinValue(uint8_t pinNumber, uint8_t val) {
	*pins[pinNumber] = val;
}

uint8_t Element::getPinValue(uint8_t pinNumber) {
	return *pins[pinNumber];
}

uint8_t* Element::getPin(uint8_t pinNumber) {
	return pins[pinNumber];
}

const vector<uint8_t*>& Element::getPins() const {
	return pins;
}
