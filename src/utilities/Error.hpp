/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Error.hpp
 * @since     Jun 6, 2018
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

#include "Defaults.hpp"

#pragma once

namespace LEDSpicer::Utilities {

/**
 * LEDSpicer::Error
 *
 * Handles error messages and can be thrown as an exception.
 */
class Error {

public:

	/**
	 * Constructs an empty Error.
	 */
	Error() = default;

	/**
	 * Constructs an Error with an initial message.
	 * @param msg
	 */
	Error(const string& msg) : message(msg) {}

	/**
	 * Constructs an Error with an initial message.
	 * @param msg
	 */
	Error(const char* msg) : message(msg) {}

	// Copy constructor.
	Error(const Error&) = default;

	// Move constructor.
	Error(Error&&) = default;

	virtual ~Error() = default;

	/**
	 * Appends content to the error message using streaming.
	 * @tparam T
	 * @param value
	 * @return
	 */
	template <typename T>
	Error& operator<<(const T& value) {
		std::ostringstream oss;
		oss << value;
		message += oss.str();
		return *this;
	}

	/**
	 * Returns the error message as a C-string.
	 * @return
	 */
	const char* what() const noexcept {
		return message.c_str();
	}

	/**
	 * Returns error message as a string.
	 * @return
	 */
	string getMessage() const {
		return message;
	}

	friend std::ostream& operator<<(std::ostream& os, const Error& e) {
		os << e.what();
		return os;
	}

protected:

	/// The error message.
	string message;

};

} // namespace
