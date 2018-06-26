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

	for (auto a : animations) {
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
	cout << "Default Color when On: ";
	defaultColorOn.drawColor();
	cout << "Default Color when Off: ";
	defaultColorOff.drawColor();
	cout << "Start Animation: " << (startAnimation.size() ? "" : "None") << endl;
	for (auto s : startAnimation)
		s->drawConfig();
	cout << "Stop Animation: " << (stopAnimation.size() ? "" : "None") << endl;
	for (auto s : stopAnimation)
		s->drawConfig();
	if (groupsOverwrite.size()) {
		cout << "Groups Overwrite Color: " << endl;
		for (auto g : groupsOverwrite) {
			cout << g.first << " ";
			g.second->drawColor();
		}
	}
	if (elementsOverwrite.size()) {
		cout << "Elements Overwrite Color: " << endl;
		for (auto e : groupsOverwrite) {
			cout << e.first << " ";
			e.second->drawColor();
		}
	}
}
