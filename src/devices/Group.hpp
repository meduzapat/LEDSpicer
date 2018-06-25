/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file		Group.hpp
 * @since		Jun 22, 2018
 * @author		Patricio A. Rossi (MeduZa)
 * @copyright	Copyright © 2018 Patricio A. Rossi (MeduZa)
 */

// To handle unordered map.
#include <unordered_map>
#ifndef umap
	#define umap std::unordered_map
#endif

#include "Element.hpp"

#ifndef LAYOUT_HPP_
#define LAYOUT_HPP_ 1

namespace LEDSpicer {
namespace Devices {

/**
 * LEDSpicer::Devices::Layout
 * A group of elements.
 */
class Group {

public:

	enum States {Color, Animation, Pattern};

	//uint8_t defaultState, const string& stateValue, 		defaultState(defaultState),	stateValue(stateValue),
	Group() {}

	virtual ~Group() {}

	void drawElements();

	vector<Element> elements;

//	uint8_t defaultState;

};

}} /* namespace LEDSpicer */

#endif /* LAYOUT_HPP_ */
