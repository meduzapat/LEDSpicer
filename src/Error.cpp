/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file		Error.cpp
 * @since		Jun 6, 2018
 * @author		Patricio A. Rossi (MeduZa)
 * @copyright	Copyright © 2018 Patricio A. Rossi (MeduZa)
 */

#include "Error.hpp"

using namespace LEDSpicer;

bool Error::strError = false;

Error::Error(const string& errorMessage) {
	// TODO Auto-generated constructor stub

}

Error::~Error() {
	// TODO Auto-generated destructor stub
}

void Error::setStdErrorOutput(bool option) {
	strError = option;
}

string Error::getMessage() {
	return std::move(error);
}
