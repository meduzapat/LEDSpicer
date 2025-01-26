/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Messages.hpp
 * @since     Jul 8, 2018
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2018 - 2025 Patricio A. Rossi (MeduZa)
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

#include <queue>
using std::queue;

#include "utility/Socks.hpp"
#include "utility/Utility.hpp"
#include "Message.hpp"

#ifndef MESSAGES_HPP_
#define MESSAGES_HPP_ 1

namespace LEDSpicer {

/**
 * LEDSpicer::Messages
 */
class Messages : protected Socks {

public:

	Messages(const string& port) : Socks(LOCALHOST, port, true) {}

	virtual ~Messages() = default;

	Message getMessage();

	bool read();

protected:

	queue<Message> messages;

};

} /* namespace LEDSpicer */

#endif /* MESSAGES_HPP_ */
