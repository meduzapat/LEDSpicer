/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Error.hpp
 * @since     Jun 6, 2018
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2018 - 2020 Patricio A. Rossi (MeduZa)
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

#include <string>
using std::string;
using std::to_string;
using std::stoi;

// Compiler settings.
#include "config.h"

#ifndef ERROR_HPP_
#define ERROR_HPP_ 1

namespace LEDSpicer {

/**
 * LEDSpicer::Error
 *
 * Handles error messages.
 */
class Error {

public:

	Error(const string& errorMessage) : error(errorMessage) {}

	virtual ~Error() = default;

	/**
	 * Returns and resets the error message.
	 * @return
	 */
	string getMessage();

protected:

	string error;

};

} /* namespace LEDSpicer */

#endif /* ERROR_HPP_ */
