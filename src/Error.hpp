/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file		Error.hpp
 * @since		Jun 6, 2018
 * @author		Patricio A. Rossi (MeduZa)
 * @copyright	Copyright © 2018 Patricio A. Rossi (MeduZa)
 */

#ifndef ERROR_HPP_
#define ERROR_HPP_ 1

// To handle IO stream.
#include <iostream>
using std::cout;
using std::cerr;
using std::endl;

// To handle strings
#include <string>
using std::string;

namespace LEDSpicer {

/**
 * LEDSpicer::Error
 *
 * Handles error messages.
 */
class Error {

public:

	Error(const string& errorMessage);

	virtual ~Error();

	/**
	 * Returns and resets the error message.
	 * @return
	 */
	string getMessage();

	/**
	 * Force the output to the standard error output instead of the log file.
	 * @param option
	 */
	static void setStdErrorOutput(bool option);

protected:

	static bool strError;

	string error;

};

} /* namespace LEDSpicer */

#endif /* ERROR_HPP_ */
