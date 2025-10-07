/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Log.cpp
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

#include "Log.hpp"

using namespace LEDSpicer::Utilities;

// Default to notice
int Log::minLevel = LOG_NOTICE;

void (*Log::logFn)(const string&, int) = Log::logIntoStdOut;

void Log::initialize(const bool logToStdOut) {
	logToStdTerm(logToStdOut);
}

void Log::log(const string& message, const int level) {
	logFn(message, level);
}

void Log::error(const string& message) {
	log(message, LOG_ERR);
}

void Log::warning(const string& message) {
	log(message, LOG_WARNING);
}

void Log::info(const string& message) {
	log(message, LOG_INFO);
}

void Log::notice(const string& message) {
	log(message, LOG_NOTICE);
}

void Log::debug(const string& message) {
	log(message, LOG_DEBUG);
}

void Log::logToStdTerm(const bool option) {

	if (not option and logFn != logIntoSysLog) {
		openlog(PROJECT_NAME, LOG_PID|LOG_CONS, LOG_USER);
		logFn = logIntoSysLog;
		return;
	}

	if (option and logFn != logIntoStdOut) {
		terminate();
		logFn = logIntoStdOut;
	}
}

void Log::logIntoSysLog(const string& message, const int level) {
	syslog(level, "%s", message.c_str());
}

void Log::logIntoStdOut(const string& message, const int level) {

	if (level > minLevel)
		return;

	if (level == LOG_ERR)
		cerr << message << endl;
	else
		cout << message << endl;
}

void Log::terminate() {
	if (logFn == logIntoSysLog) {
		logFn = nullptr;
		closelog();
	}
}

void Log::setLogLevel(const int level) {
	minLevel = level;
	if (logFn == logIntoSysLog)
		setlogmask(LOG_UPTO(level));
}

int Log::getLogLevel() {
	return minLevel;
}

bool Log::isLogging(const int level) {
	return level <= minLevel;
}

int Log::str2level(const string& level) {
	if (level == "Error")
		return LOG_ERR;
	if (level == "Warning")
		return LOG_WARNING;
	if (level == "Notice")
		return LOG_NOTICE;
	if (level == "Info")
		return LOG_INFO;
	if (level == "Debug")
		return LOG_DEBUG;
	LogError("Invalid log level " + level + " assuming Info");
	return LOG_INFO;
}

const string Log::level2str(const int level) {
	switch (level) {
	case LOG_ERR:
		return "Error";
	case LOG_WARNING:
		return "Warning";
	case LOG_NOTICE:
		return "Notice";
	case LOG_INFO:
		return "Info";
	case LOG_DEBUG:
		return "Debug";
	}
	return "Unknown";
}
