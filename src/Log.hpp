/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file		Log.hpp
 * @since		Jun 7, 2018
 * @author		Patricio A. Rossi (MeduZa)
 * @copyright	Copyright © 2018 Patricio A. Rossi (MeduZa)
 */

#ifndef LOG_HPP_
#define LOG_HPP_ 1

// To handle IO stream.
#include <iostream>
using std::cout;
using std::cerr;
using std::endl;

// To handle strings
#include <string>
using std::string;

#include <syslog.h>

#include "config.h"

namespace LEDSpicer {

/**
 * LEDSpicer::Log
 * Static logger functionality.
 */
class Log {

public:

	virtual ~Log() {}

	/**
	 * Function to initialize the logs, other case they will output to the terminal.
	 * @param logToStdOut
	 */
	static void initialize(bool logToStdOut);

	/**
	 * Changes the logging destination.
	 * @param option Enables/disables the logging into the terminal.
	 */
	static void logToStdErr(bool option = false);

	/**
	 * Logs errors.
	 * @param message the message to log
	 */
	static void error(const string& message);

	/**
	 * Logs warnings.
	 * @param message the message to log
	 */
	static void warning(const string& message);

	/**
	 * Logs info.
	 * @param message the message to log
	 */
	static void info(const string& message);

	/**
	 * Logs notice.
	 * @param message the message to log
	 */
	static void notice(const string& message);

	/**
	 * Logs debug.
	 * @param message the message to log
	 */
	static void debug(const string& message);

	/**
	 * Destroys the logger.
	 */
	static void terminate();

protected:

	/**
	 * logging function pointer.
	 */
	static void (*logFn)(const string&, int);

	/**
	 * Creates a new Log object
	 */
	Log();

	/**
	 * Function to log into the syslog
	 * @param message
	 * @param level
	 */
	static void logToSysLog(const string& message, int level);

	/**
	 * Function to log into the standard output
	 * @param message
	 * @param level
	 */
	static void logToStdOut(const string& message, int level);

	/**
	 * Creates a log entry
	 * @param message the message to log
	 * @param level the log level (LOG_ERR, LOG_WARNING, etc)
	 */
	static void log(const string& message, int level);


};

} /* namespace LEDSpicer */

#endif /* LOG_HPP_ */
