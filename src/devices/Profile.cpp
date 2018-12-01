/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Profile.cpp
 * @since     Jun 25, 2018
 * @author    Patricio A. Rossi (MeduZa)
 * @copyright Copyright © 2018 Patricio A. Rossi (MeduZa)
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

	if (alwaysOnGroups.size()) {
		cout << endl << "Groups Overwrite Color: " << endl;
		for (auto& g : alwaysOnGroups) {
			cout << g.group->getName() << " ";
			g.color->drawColor();
		}
	}
	if (alwaysOnElements.size()) {
		cout << endl << "Elements Overwrite Color: " << endl;
		for (auto& e : alwaysOnElements) {
			cout << e.element->getName() << " ";
			e.color->drawColor();
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
	actual = nullptr;
	restartActors();
}

void Profile::restart() {

	running = true;
	restartActors();

	if (start) {
		actual = start;
		actual->restart();
	}
}

void Profile::terminate() {
	if (end) {
		actual = end;
		actual->restart();
	}
	running = end != nullptr;
}

void Profile::restartActors() {
	for (auto& anim : animations)
		for (auto actor : anim.second)
			actor->restart();
}

bool Profile::isTransiting() const {
	return (actual == start or actual == end) and actual != nullptr;
}

bool Profile::isTerminating() const {
	return actual == end and actual != nullptr;
}

bool Profile::isStarting() const {
	return actual == start and actual != nullptr;
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

const string& Profile::getName() const {
	return name;
}

const vector<Profile::ElementItem>& Profile::getAlwaysOnElements() const {
	return alwaysOnElements;
}

void Profile::addAlwaysOnElement(Element* element ,const string& color) {
	alwaysOnElements.push_back(ElementItem{element, &Color::getColor(color), Color::Filters::Normal});
}

const vector<Profile::GroupItem> & Profile::getAlwaysOnGroups() const {
	return alwaysOnGroups;
}

void Profile::addAlwaysOnGroup(Group* group, const string& color) {
	alwaysOnGroups.push_back(GroupItem{group, &Color::getColor(color), Color::Filters::Normal});
}
