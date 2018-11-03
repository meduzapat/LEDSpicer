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

#include "Message.hpp"

#ifndef EMITTER_HPP_
#define EMITTER_HPP_ 1

namespace LEDSpicer {

/**
 * LEDSpicer::Emitter
 */
class Emitter {

public:

	Emitter() {
		//"127.0.0.1", "16161"

	}

	virtual ~Emitter() {
	}

};

/**
 * Main function.
 * Handles command line and executes the program.
 *
 * @param argc
 * @param argv
 * @return exit code.
 */
int main(int argc, char **argv);

}
/* namespace LEDSpicer */

#endif /* EMITTER_HPP_ */
