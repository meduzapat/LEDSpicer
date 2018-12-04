/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Messages.hpp
 * @since     Jul 8, 2018
 * @author    Patricio A. Rossi (MeduZa)
 * @copyright Copyright © 2018 Patricio A. Rossi (MeduZa)
 */

#include <queue>
using std::queue;

#include "utility/Socks.hpp"
#include "utility/Utility.hpp"
#include "Message.hpp"

#ifndef MESSAGES_HPP_
#define MESSAGES_HPP_ 1

#define LOCALHOST "127.0.0.1"

namespace LEDSpicer {

/**
 * LEDSpicer::Messages
 */
class Messages : protected Socks {

public:

	Messages(const string& port) : Socks(LOCALHOST, port, true) {}

	virtual ~Messages() {}

	Message getMessage();

	bool read();

protected:

	queue<Message> messages;

};

} /* namespace LEDSpicer */

#endif /* MESSAGES_HPP_ */
