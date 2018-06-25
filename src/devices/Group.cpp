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
		cout << std::left << std::setfill(' ') << std::setw(15) << element.name <<
			" Pin" << (element.pins.size() == 1 ? " " : "s") <<  ": ";
		for (auto pin : element.getPins()) {
			cout << (int)*pin << " ";
		}
		cout << endl;
	}
}
