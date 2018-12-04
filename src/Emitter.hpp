/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Emitter.hpp
 * @since     Jul 8, 2018
 * @author    Patricio A. Rossi (MeduZa)
 * @copyright Copyright © 2018 Patricio A. Rossi (MeduZa)
 */

#include <sys/socket.h>
#include <netdb.h>

#include <iostream>
using std::cout;
using std::endl;

#include <unistd.h>

#include <cstring>

#include <memory>

#include "Messages.hpp"
#include "utility/XMLHelper.hpp"

#ifndef EMITTER_HPP_
#define EMITTER_HPP_ 1

#define CONFIG_FILE PACKAGE_CONF_DIR "/" PACKAGE ".conf"

/**
 * Main function.
 * Handles command line and executes the program.
 *
 * @param argc
 * @param argv
 * @return exit code.
 */
int main(int argc, char **argv);

vector<string> parseMame(const string& binary, const string& rom);

#endif /* EMITTER_HPP_ */
