/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file		Group.cpp
 * @since		Jun 22, 2018
 * @author		Patricio A. Rossi (MeduZa)
 * @copyright	Copyright © 2018 Patricio A. Rossi (MeduZa)
 */

#include "Group.hpp"

using namespace LEDSpicer::Devices;

void Group::drawElements() {
	cout << (int)elements.size() << " Element(s): " << endl;
	for (auto element : elements) {
		cout << std::left << std::setfill(' ') << std::setw(15) << element->getName() <<
			" Pin" << (element->getPins().size() == 1 ? " " : "s") <<  ": ";
		for (auto pin : element->getPins()) {
			cout << (int)*pin << " ";
		}
		cout << endl;
	}
}

uint8_t Group::size() const {
	return elements.size();
}

void Group::linkElement(Element* element) {
	elements.push_back(element);
}

const vector<Element*>& Group::getElements() const {
	return elements;
}

Element* Group::getElement(uint8_t index) {
	return elements.at(index);
}

/*
void Group::writeBuffer(Color::Filters filter) {
	for (uint8_t eleC = 0; eleC < elements.size(); ++eleC) {
		for (uint8_t pinC = 0; pinC < elements[eleC]->getPins().size(); ++pinC) {
			switch (filter) {
			case Color::Filters::Normal:
				elements[eleC]->setPinValue(pinC, internalCopy[eleC].getPinValue(pinC));
			break;
			case Color::Filters::Combine:
				elements[eleC]->setPinValue(
					pinC,
					Color::transition(
						elements[eleC]->getPinValue(pinC),
						internalCopy[eleC].getPinValue(pinC),
						50
					)
				);
			}
		}
	}
}

*/
