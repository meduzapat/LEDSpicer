/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Handler.hpp
 * @since     Dec 8, 2018
 * @author    Patricio A. Rossi (MeduZa)
 * @copyright Copyright © 2018 - 2019 Patricio A. Rossi (MeduZa)
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

	Handler(const string& filename);

	virtual ~Handler();

protected:

	void* handler = nullptr;

};

} /* namespace LEDSpicer */

#endif /* HANDLER_HPP_ */
