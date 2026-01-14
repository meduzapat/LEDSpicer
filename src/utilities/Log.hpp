/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Log.hpp
 * @since     Jun 7, 2018
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2018 - 2026 Patricio A. Rossi (MeduZa)
 *
 * @copyright LEDSpicer is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * @copyright LEDSpicer is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * @copyright You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */


#include <syslog.h>

#include "Defaults.hpp"

#pragma once

#define LogError(message)   if (Log::isLogging(LOG_ERR))     {Log::error(message);}
#define LogWarning(message) if (Log::isLogging(LOG_WARNING)) {Log::warning(message);}
#define LogNotice(message)  if (Log::isLogging(LOG_NOTICE))  {Log::notice(message);}
#define LogInfo(message)    if (Log::isLogging(LOG_INFO))    {Log::info(message);}
#define LogDebug(message)   if (Log::isLogging(LOG_DEBUG))   {Log::debug(message);}

namespace LEDSpicer::Utilities {

/**
 * LEDSpicer::Log
 * Static logger functionality.
 */
class Log {

public:

	virtual ~Log() = default;

	/**
	 * Function to initialize the logs, other case they will output to the terminal.
	 * @param logToStdOut
	 */
	static void initialize(const bool logToStdOut);

	/**
	 * Changes the logging destination.
	 * @param option Enables/disables the logging into the terminal.
	 */
	static void logToStdTerm(const bool option = false);

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

	/**
	 * Sets the log level to minimal following  this table:
	 *  LOG_EMERG
	 *  LOG_ALERT
	 *  LOG_CRIT
	 *  LOG_ERR
	 *  LOG_WARNING
	 *  LOG_NOTICE
	 *  LOG_INFO
	 *  LOG_DEBUG
	 * @param level
	 */
	static void setLogLevel(const int level);

	/**
	 * Returns the current in use log level.
	 * @return
	 */
	static int getLogLevel();

	/**
	 * returns true if the logger is logging the level.
	 * @param level
	 * @return
	 */
	static bool isLogging(const int level);

	/**
	 * Converts a string log level into its numeric value.
	 * @param level
	 * @return
	 */
	static int str2level(const string& level);

	/**
	 * Converts a log level value into its string representation.
	 * @param level
	 * @return
	 */
	static const string level2str(const int level);

protected:

	/**
	 * The level of logging.
	 */
	static int minLevel;

	/**
	 * logging function pointer.
	 */
	static void (*logFn)(const string&, int);

	/**
	 * Function to log into the syslog
	 * @param message
	 * @param level
	 */
	static void logIntoSysLog(const string& message,const  int level);

	/**
	 * Function to log into the standard output
	 * @param message
	 * @param level
	 */
	static void logIntoStdOut(const string& message,const  int level);

	/**
	 * Creates a log entry
	 * @param message the message to log
	 * @param level the log level (LOG_ERR, LOG_WARNING, etc)
	 */
	static void log(const string& message,const  int level);

private:

	/**
	 * Static class.
	 */
	Log() = delete;

	/**
	 * Disabled.
	 * @param
	 */
	Log(Log const&) = delete;

	/**
	 * Disabled.
	 * @param other
	 * @return
	 */
	Log& operator=(const Log& other) = delete;

	/**
	 * Disabled.
	 * @param other
	 * @return
	 */
	Log& operator=(Log&& other) noexcept = delete;

};

} // namespace
