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

	Group() {}

	Group(const vector<Element*>& elements) : elements(elements) {}

	virtual ~Group() {}

	void drawElements();

	/**
	 * Returns the number of elements.
	 * @return
	 */
	uint8_t size() const;

	/**
	 * Inserts a new element.
	 * @param element
	 */
	void linkElement(Element* element);

	/**
	 * Returns a reference to the pointers to the internal elements.
	 * @return
	 */
	const vector<Element*>& getElements() const;

	/**
	 * Returns a reference to the internal element.
	 * @param index
	 * @return
	 */
	Element* getElement(uint8_t index);

	/**
	 * Writes the internal buffer.
	 */
//	void writeBuffer(Color::Filters filter);

protected:

	/// Elements on this group.
	vector<Element*> elements;

};

}} /* namespace LEDSpicer */

#endif /* LAYOUT_HPP_ */
