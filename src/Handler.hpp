/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Handler.hpp
 * @since     Dec 8, 2018
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2018 - 2019 Patricio A. Rossi (MeduZa)
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

#include "config.h"

#include <dlfcn.h>

#include "utility/Log.hpp"
#include "utility/Error.hpp"

#ifndef HANDLER_HPP_
#define HANDLER_HPP_ 1

namespace LEDSpicer {

/**
 * LEDSpicer::Handler
 */
class Handler {

public:

	Handler() = default;

	/**
	 * Loads a new plugin using its library name.
	 * @param the plugin library name without extension or path.
	 */
	Handler(const string& filename);

	/**
	 * releases the linked library.
	 */
	virtual ~Handler();

protected:

	/// Pointer to the dynamic linked library.
	void* handler = nullptr;

};

} /* namespace LEDSpicer */

#endif /* HANDLER_HPP_ */
