/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file		Fill.hpp
 * @ingroup
 * @since		Jul 21, 2018
 * @author		Patricio A. Rossi (MeduZa)
 * @copyright	Copyright © 2018 Patricio A. Rossi (MeduZa)
 */

#include "Actor.hpp"

#ifndef FILL_HPP_
#define FILL_HPP_ 1

#define REQUIRED_PARAM_ACTOR_FILL {"colors"}

namespace LEDSpicer {
namespace Animations {

/**
 * LEDSpicer::Fill
 */
class Fill: public Animations::Actor {

public:

	Fill(umap<string, string>& parameters, Group* const layout);

	virtual ~Fill() {}

protected:

	void calculateElements();

	vector<const Color*> colors;

};

}} /* namespace LEDSpicer */

#endif /* FILL_HPP_ */
