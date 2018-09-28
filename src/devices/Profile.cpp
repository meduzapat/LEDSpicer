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

	for (auto& a : animations) {
		for (auto actor : a.second)
			delete actor;
		a.second.clear();
	}
	animations.clear();

	if (start)
		delete start;

	if (end)
		delete end;
}

void Profile::addAnimation(const string& animationName, const vector<Actor*>& animation) {
	animations[animationName] = std::move(animation);
}

void Profile::drawConfig() {
	cout << "Background color: ";
	backgroundColor.drawColor();
	if (start) {
		cout << endl << "Start transition:" << endl;
		start->drawConfig();
	}
	if (end) {
		cout << endl << "Ending transition:" << endl;
		end->drawConfig();
	}

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

void Profile::runFrame() {

	if (actual and actual->drawFrame()) {
		if (actual == end)
			running = false;
		actual = nullptr;
	}

	if (not actual and running)
		for (auto& anim : animations)
			for (auto actor : anim.second)
				actor->drawFrame();
}

void Profile::reset() {
	running = true;
	actual = start;
	if (start)
		actual->restart();
}

void Profile::terminate() {
	running = true;
	actual = end;
	if (end)
		actual->restart();
}

bool Profile::isTransiting() const {
	return actual == start or actual == end;
}

bool Profile::isRunning() const {
	return running;
}

const LEDSpicer::Color& Profile::getBackgroundColor() const {
	return backgroundColor;
}

uint8_t Profile::getAnimationsCount() const {
	return animations.size();
}
