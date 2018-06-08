/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file		Log.cpp
 * @since		Jun 7, 2018
 * @author		Patricio A. Rossi (MeduZa)
 * @copyright	Copyright © 2018 Patricio A. Rossi (MeduZa)
 */

#include "Log.hpp"

using namespace LEDSpicer;

void (*Log::logFn)(const string&, int) = Log::logToStdOut;

void Log::initialize(bool logToStdOut) {
	logToStdErr(logToStdOut);
}

void Log::log(const string& message, int level) {
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
#ifdef DEVELOP
	log(message, LOG_DEBUG);
#endif
}

void Log::logToStdErr(bool option) {

	if (not option and logFn != logToSysLog) {
		openlog(PACKAGE_NAME, LOG_PID|LOG_CONS, LOG_USER);
		logFn = logToSysLog;
		return;
	}

	if (option and logFn != logToStdOut) {
		terminate();
		logFn = logToStdOut;
	}
}

void Log::logToSysLog(const string& message, int level) {
	syslog(level, "%s", message.c_str());
}

void Log::logToStdOut(const string& message, int level) {
	if (level == LOG_ERR) {
		cerr << message << endl;
		return;
	}
	cout << message << endl;
}

void Log::terminate() {
	if (logFn == logToSysLog) {
		logFn = nullptr;
		closelog();
	}
}
