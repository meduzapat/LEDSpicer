/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file		Profile.hpp
 * @since		Jun 25, 2018
 * @author		Patricio A. Rossi (MeduZa)
 * @copyright	Copyright © 2018 Patricio A. Rossi (MeduZa)
 */

#ifndef PROFILE_HPP_
#define PROFILE_HPP_ 1

#include "Element.hpp"
#include "Group.hpp"
#include "../animations/Actor.hpp"
#include "../utility/Color.hpp"

namespace LEDSpicer {
namespace Devices {

using Animations::Actor;

/**
 * LEDSpicer::Profile
 */
class Profile {

public:

	enum class States : uint8_t {Starting, Running, Ending, Done};

//	Profile() {}

	Profile(
		const Color& defaultColorOn,
		const Color& defaultColorOff,
		const vector<Actor*>& startAnimation,
		const vector<Actor*>& stopAnimation
	) :
		defaultColorOn(std::move(defaultColorOn)),
		defaultColorOff(std::move(defaultColorOff)),
		startAnimation(std::move(startAnimation)),
		stopAnimation(std::move(stopAnimation)) {}

	virtual ~Profile();

	void addAnimation(const string& animationName, const vector<Actor*>& animation);

	void drawConfig();

	/**
	 * Execute a frame.
	 * @return
	 */
	const Profile::States runFrame();

	/**
	 * Set the state back to Starting.
	 */
	void resetState();

protected:

	/// State 0 starting, 1 running, 2 ending, 3 done.
	States state = States::Starting;

	/// Flag to know if the start or stop animation (if any) is executing.
	bool runningStartStopOnly = true;

	Color
		defaultColorOn,
		defaultColorOff;

	vector<Actor*>
		/// Initial animation.
		startAnimation,
		/// Final animation.
		stopAnimation;

	/// List of animations to run.
	umap<string, vector<Actor*>> animations;

	/// List of Elements by name that need the color to be changed.
	umap<string,const Color* const> elementsOverwrite;

	/// List of Groups by name that need the color to be changed.
	umap<string,const Color* const> groupsOverwrite;


};

}} /* namespace LEDSpicer */

#endif /* PROFILE_HPP_ */
