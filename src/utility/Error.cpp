/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Error.cpp
 * @since     Jun 6, 2018
 * @author    Patricio A. Rossi (MeduZa)
 * @copyright Copyright © 2018 - 2019 Patricio A. Rossi (MeduZa)
 */

#include "Error.hpp"

using namespace LEDSpicer;

const char* const Error::SIGTERM_LABEL             = PACKAGE_NAME " terminated by signal";
const char* const Error::SIGHUP_LABEL              = PACKAGE_NAME " received re load configuration signal";
const char* const Error::RUNNING_LABEL             = PACKAGE_NAME " Running";
const char* const Error::INVALID_MESSAGE_LABEL     = "Invalid message ";
const char* const Error::INVALID_ELEMENT_LABEL     = "Unknown element ";
const char* const Error::INVALID_GROUP_LABEL       = "Unknown group ";
const char* const Error::TERMINATED_BY_ERROR_LABEL = "Program terminated by error: ";

string Error::getMessage() {
	return std::move(error);
}
