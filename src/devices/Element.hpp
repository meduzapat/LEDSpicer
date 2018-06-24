/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file		Element.hpp
 * @since		Jun 22, 2018
 * @author		Patricio A. Rossi (MeduZa)
 * @copyright	Copyright © 2018 Patricio A. Rossi (MeduZa)
 */

// For ints.
#include <cstdint>

// To handle dynamic arrays.
#include <vector>
using std::vector;

#include "../utility/Color.hpp"

#ifndef ELEMENT_HPP_
#define ELEMENT_HPP_ 1

namespace LEDSpicer {
namespace Devices {

/**
 * LEDSpicer::Devices::Element
 * Interface between animations and one or three pins on the board.
 */
class Element {

public:

	//enum Types : uint8_t {button, joystick, lightBar, trackball, light, marquee}

	Element() {}

	virtual ~Element() {}

	void setColor(Color color);

	vector<uint8_t*> pins;

	string name;

};

}} /* namespace LEDSpicer */

#endif /* ELEMENT_HPP_ */
