/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Handler.cpp
 * @since     Dec 8, 2018
 * @author    Patricio A. Rossi (MeduZa)
 * @copyright Copyright © 2018 Patricio A. Rossi (MeduZa)
 */

#include "Handler.hpp"

using namespace LEDSpicer;

Handler::Handler(const string& filename) :handler(dlopen(filename.c_str(), RTLD_LAZY)) {
	LogDebug("Loading library " + filename);
	if (char *errstr = dlerror())
		throw Error("Failed to load library " + filename + " " + errstr);
}

Handler::~Handler() {
	if (not handler)
		return;
	dlclose(handler);
	handler = nullptr;
}
