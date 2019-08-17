/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      InputHandler.hpp
 * @since     May 7, 2019
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

#include "Input.hpp"
#include "Handler.hpp"

#ifndef INPUTSHANDLER_HPP_
#define INPUTSHANDLER_HPP_ 1

namespace LEDSpicer {
namespace Inputs {

/**
 * LEDSpicer::Inputs::InputHandler
 * This is an Input factory to loader and create Input plugins.
 */
class InputHandler: public Handler {

public:

	InputHandler() = default;

	/**
	 * @see Handler::Handler()
	 */
	InputHandler(const string& inputName);

	virtual ~InputHandler() = default;

	/**
	 * Wrapper over the create pointer.
	 * @see createFunction pointer.
	 */
	Input* createInput(umap<string, string>& parameters);

	/**
	 * Wrapper over the destroy pointer.
	 * @see destroyFunction pointer.
	 */
	void destroyInput(Input* input);

protected:

	/**
	 * Pointer to the plugin's creation function.
	 * @param plugin parameters.
	 * @return a new created plugin.
	 */
	Input*(*createFunction)(umap<string, string>&) = nullptr;

	/**
	 * Pointer to the plugin's destruction function.
	 * @param pointer to the plugin to destroy.
	 */
	void(*destroyFunction)(Input*) = nullptr;
};

} /* namespace Inputs */
} /* namespace LEDSpicer */

#endif /* INPUTSHANDLER_HPP_ */
