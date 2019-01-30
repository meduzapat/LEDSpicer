/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Error.hpp
 * @since     Jun 6, 2018
 * @author    Patricio A. Rossi (MeduZa)
 * @copyright Copyright © 2018 - 2019 Patricio A. Rossi (MeduZa)
 */

#include <string>
using std::string;
using std::to_string;
using std::stoi;

// Compilation settings.
#include "config.h"

#ifndef ERROR_HPP_
#define ERROR_HPP_ 1

namespace LEDSpicer {

/**
 * LEDSpicer::Error
 *
 * Handles error messages.
 */
class Error {

public:

	static const char* const SIGTERM_LABEL;
	static const char* const SIGHUP_LABEL;
	static const char* const RUNNING_LABEL;
	static const char* const INVALID_MESSAGE_LABEL;
	static const char* const INVALID_ELEMENT_LABEL;
	static const char* const INVALID_GROUP_LABEL;
	static const char* const TERMINATED_BY_ERROR_LABEL;

	Error(const string& errorMessage) : error(errorMessage) {}

	virtual ~Error() {}

	/**
	 * Returns and resets the error message.
	 * @return
	 */
	string getMessage();

protected:

	string error;

};

} /* namespace LEDSpicer */

#endif /* ERROR_HPP_ */
