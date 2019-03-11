/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Group.hpp
 * @since     Jun 22, 2018
 * @author    Patricio A. Rossi (MeduZa)
 * @copyright Copyright © 2018 - 2019 Patricio A. Rossi (MeduZa)
 */

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

	Group() = default;

	Group(const string& name) : name(name) {}

	virtual ~Group() = default;

	void drawElements();

	/**
	 * @return the number of elements.
	 */
	uint8_t size() const;

	/**
	 * Inserts a new element.
	 * @param element
	 */
	void linkElement(Element* element);

	/**
	 * Reduces the group elements to the minimum necessary.
	 */
	void shrinkToFit();

	/**
	 * @return a reference to the pointers to the internal elements.
	 */
	const vector<Element*>& getElements() const;

	/**
	 * @param index
	 * @return a reference to the internal element.
	 */
	Element* getElement(uint8_t index);

	/**
	 * @return the group name.
	 */
	const string& getName() const;

protected:

	/// Elements on this group.
	vector<Element*> elements;

	string name = "";

};

}} /* namespace LEDSpicer */

#endif /* LAYOUT_HPP_ */
