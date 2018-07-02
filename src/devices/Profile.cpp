/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file		Profile.cpp
 * @since		Jun 25, 2018
 * @author		Patricio A. Rossi (MeduZa)
 * @copyright	Copyright © 2018 Patricio A. Rossi (MeduZa)
 */

#include "Profile.hpp"

using namespace LEDSpicer::Devices;

Profile::~Profile() {

	for (auto s : startAnimation)
		delete s;
	startAnimation.clear();

	for (auto s : stopAnimation)
		delete s;
	stopAnimation.clear();

	for (auto& a : animations) {
		for (auto actor : a.second)
			delete actor;
		a.second.clear();
	}
	animations.clear();

}

void Profile::addAnimation(const string& animationName, const vector<Actor*>& animation) {
	animations[animationName] = std::move(animation);
}

void Profile::drawConfig() {
	cout << "Default Color when On:  ";
	defaultColorOn.drawColor();
	cout << "Default Color when Off: ";
	defaultColorOff.drawColor();
	cout << endl << "Start Animation: " << (startAnimation.size() ? "" : "None") << endl;
	for (auto s : startAnimation)
		s->drawConfig();
	cout << endl << "Stop Animation: " << (stopAnimation.size() ? "" : "None") << endl;
	for (auto s : stopAnimation)
		s->drawConfig();

	for (auto& a : animations) {
		cout << endl << "Animation " << a.first << ": " << endl;
		for(auto actor : a.second)
			actor->drawConfig();
	}
	if (groupsOverwrite.size()) {
		cout << endl << "Groups Overwrite Color: " << endl;
		for (auto& g : groupsOverwrite) {
			cout << g.first << " ";
			g.second->drawColor();
		}
	}
	if (elementsOverwrite.size()) {
		cout << endl << "Elements Overwrite Color: " << endl;
		for (auto& e : elementsOverwrite) {
			cout << e.first << " ";
			e.second->drawColor();
		}
	}
}

const Profile::States Profile::runFrame() {

	for (auto& anim : animations)
		for (auto actor : anim.second)
			actor->drawFrame();
	return States::Running;


	uint8_t status;
	if (state == States::Starting)
		if (startAnimation.size()) {
			for (auto actor : startAnimation) {
				status = actor->drawFrame();
				// activate other animations half way.
				if (status == actor->getTotalFrames() / 2)
					runningStartStopOnly = false;
				else if (status == actor->getTotalFrames())
					state = States::Running;
			}
		}

	if (not runningStartStopOnly)
		for (auto& anim : animations)
			for (auto actor : anim.second)
				actor->drawFrame();

	if (state == States::Ending)
		if (stopAnimation.size()) {
			for (auto actor : stopAnimation) {
				status = actor->drawFrame();
				if (status == actor->getTotalFrames() / 2)
					// de-activate other animations half way.
					runningStartStopOnly = true;
				else if (status == actor->getTotalFrames())
					state = States::Done;
			}
		}
	return state;
}

void Profile::resetState() {
	state = States::Starting;
}

