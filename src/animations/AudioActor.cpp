/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      AudioActor.cpp
 * @since     Oct 6, 2020
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2020 Patricio A. Rossi (MeduZa)
 *
 * @copyright LEDSpicer is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * @copyright LEDSpicer is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * @copyright You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "AudioActor.hpp"

using namespace LEDSpicer::Animations;

uint8_t AudioActor::lastF  = 250;
uint8_t AudioActor::lastFs = 250;
AudioActor::Values AudioActor::value;
vector<AudioActor::Values> AudioActor::values;

AudioActor::AudioActor(umap<string, string>& parameters, Group* const group) :
	Actor(parameters, group, REQUIRED_PARAM_ACTOR_AUDIO),
	Direction(parameters.count("direction") ? parameters["direction"] : "Forward"),
	userPref({
		Color::getColor(parameters["off"]),
		Color::getColor(parameters["low"]),
		Color::getColor(parameters["mid"]),
		Color::getColor(parameters["high"]),
		str2mode(parameters["mode"]),
		str2channel(parameters["channel"])
	}),
	totalElements(str2channel(parameters["channel"]) != Channels::Both ? group->size() : group->size() / 2)
{

	switch (userPref.mode) {
	case Modes::VuMeter:
		if (group->size() < VU_MIN_ELEMETS)
			throw Error("To use VU meter a minimum of " + to_string(VU_MIN_ELEMETS) + " elements is needed on the group.");
		break;
	case Modes::Wave:
		waveData.insert(waveData.begin(), group->size(), Color());
		break;
	}

	// No bouncing here.
	if (direction == Directions::ForwardBouncing)
		direction = Directions::Forward;
	else if (direction == Directions::BackwardBouncing)
		direction = Directions::Backward;
}

void AudioActor::calculateElements() {
	switch (userPref.mode) {
	case Modes::VuMeter:
		vuMeters();
		break;
	case Modes::Single:
		single();
		break;
	case Modes::Levels:
		levels();
		break;
	case Modes::Wave:
		waves();
	}
}

void AudioActor::resetPeak() {
	if (frame == lastF)
		return;
	lastF = frame;
	value.l = value.r = 0;
}

void AudioActor::resetPeaks() {
	// Check is on read peaks
	lastFs = frame;
	values.clear();
}

#ifdef DEVELOP
void AudioActor::displayPeak() {
	cout
		<< "L:" << std::left << std::setfill(' ') << std::setw(5) << to_string(value.l)
		<< " R:" << std::setw(5) << to_string(value.r) << endl;
}
#endif

void AudioActor::single() {

	resetPeak();
	calcPeak();
#ifdef DEVELOP
	displayPeak();
#endif

	if (not value.l and not value.r)
		return;

	uint8_t e = userPref.channel == Channels::Both ? totalElements : 0;

	Color l, r;

	if (userPref.channel & Channels::Mono)
		l = detectColor((value.l + value.r) / 2);
	if (userPref.channel & Channels::Left)
		l = detectColor(value.l);
	if (userPref.channel & Channels::Right)
		r = detectColor(value.r);

	for (uint8_t c = 0; c < totalElements; ++c) {
		if (userPref.channel & Channels::Mono)
			changeElementColor(c, l, filter);
		if (userPref.channel & Channels::Left)
			changeElementColor(c, l, filter);
		if (userPref.channel & Channels::Right)
			changeElementColor(e + c, r, filter);
	}
}

void AudioActor::vuMeters() {

	/*
	 * Function to draw the elements.
	 */
	auto tintFn = [&] (uint8_t elems, uint8_t start, bool reverse) {
		for (uint8_t e = 0; e <= elems; ++e) {
			uint8_t s = reverse ? start - e : start + e;
#ifdef DEVELOP
			cout << std::setw(3) << to_string(s);
#endif
			changeElementColor(s, detectColor(round((e + 1) * 100.00 / totalElements), false), filter);
		}
	};

	resetPeak();
	calcPeak();
#ifdef DEVELOP
	displayPeak();
#endif

	if (not value.l and not value.r)
		return;

	uint16_t val;
	// Convert to mono and them to elements.
	if (userPref.channel == Channels::Mono) {
		val = totalElements * ((value.l + value.r) / 2) / 100;
		if (not val)
			return;
		if (val > totalElements)
			val = totalElements;

		--val;
		if (not val)
			return;

		if (direction == Directions::Forward)
			tintFn(val, 0, false);
		else
			tintFn(val, getNumberOfElements() - 1, true);
		return;
	}

	// Convert left channel peak to elements.
	if (userPref.channel & Channels::Left) {
		val  = totalElements * value.l / 100;
		if (val > totalElements)
			val = totalElements;
		if (val) {
			--val;
			if (val) {
				if (direction == Directions::Forward)
					tintFn(val, 0, false);
				else
					tintFn(val, totalElements, true);
			}
		}
	}

	// Convert right channel peak to elements.
	if (userPref.channel & Channels::Right) {
		val = totalElements * value.r / 100;
		if (val > totalElements)
			val = totalElements;
		if (val) {
			--val;
			if (val) {
				if (direction == Directions::Forward)
					tintFn(val, getNumberOfElements() - 1, true);
				else
					tintFn(val, totalElements, false);
			}
		}
	}
}

void AudioActor::levels() {

	if (frame != lastFs) {
		resetPeaks();
		calcPeaks();
	}

	if (values.empty())
		return;

	uint8_t e = userPref.channel == Channels::Both ? totalElements : 0;
	for (uint8_t c = 0, c2 = 0; c < totalElements; ++c, ++c2) {

		if (c2 > values.size())
			c2 = 0;

#ifdef DEVELOP
		cout
			<< "E" << std::left << std::setfill(' ') << std::setw(3) << to_string(c)
			<< " L:" << std::left << std::setfill(' ') << std::setw(5) << to_string(values[c2].l)
			<< " R:" << std::setw(5) << to_string(values[c2].r) << endl;
#endif

		if (userPref.channel & Channels::Mono)
			changeElementColor(c, detectColor((values[c2].l + values[c2].r) / 2), filter);
		if (userPref.channel & Channels::Left)
			changeElementColor(c, detectColor(values[c2].l), filter);
		if (userPref.channel & Channels::Right)
			changeElementColor(e + c, detectColor(values[c2].r), filter);
	}
}

void AudioActor::waves() {

	uint8_t
		t = waveData.size() - 1,
		h = waveData.size() / 2 - 1;
	// single/mono VS stereo
	if (userPref.channel == Channels::Both) {
		// outside in
		if (direction == Directions::Forward) {
			for (uint8_t c = 0; c < h; c++) {
				waveData[h - c] = waveData[h - c - 1];
				waveData[h + c + 1] = waveData[h + c + 2];
			}
			if (waveData.size() % 2 != 0)
				waveData[t - 1] = waveData[t];
		}
		// inside out
		else {
			for (uint8_t c = 0; c < h; c++) {
				waveData[c] = waveData[c + 1];
				waveData[t - c] = waveData[t - c - 1];
			}
			if (waveData.size() % 2 != 0)
				waveData[h + 2] = waveData[h + 1];
		}
	}
	else {
		for (uint8_t c = 0; c < t; c++)
			if (direction == Directions::Forward)
				waveData[t - c] = waveData[t - c - 1];
			else
				waveData[c] = waveData[c + 1];
	}

	resetPeak();
	calcPeak();
#ifdef DEVELOP
	displayPeak();
#endif

	switch (userPref.channel) {
	case Channels::Both:
		if (direction == Directions::Forward) {
			waveData[0] = detectColor(value.l);
			waveData[t] = detectColor(value.r);
		}
		else {
			waveData[h] = detectColor(value.l);
			waveData[h + 1] = detectColor(value.r);
		}
		break;
	case Channels::Mono:
		waveData[direction == Directions::Forward ? 0 : t] = detectColor(value.r + value.l / 2);
		break;
	case Channels::Left:
		waveData[direction == Directions::Forward ? 0 : t] = detectColor(value.l);
		break;
	case Channels::Right:
		waveData[direction == Directions::Forward ? 0 : t] = detectColor(value.r);
		break;
	}

	for (uint8_t c = 0; c < waveData.size(); ++c)
		changeElementColor(c, waveData[c], filter);
}

#define CALC_PERC(t, b) round(abs(percent - (b)) * 100.00 / ((t) - (b)))

LEDSpicer::Color AudioActor::detectColor(uint8_t percent, bool gradient) {

	// Off.
	if (not percent)
		return userPref.off;

	if (gradient) {
		if (percent == 100)
			return userPref.c75;
	} else if (percent > HIG_POINT) {
		return userPref.c75;
	}

	// Mid -> High.
	if (percent > MID_POINT)
		return userPref.c50.transition(userPref.c75, CALC_PERC((gradient ? 99 : HIG_POINT), MID_POINT));

	// Low -> Mid.
	if (percent > LOW_POINT)
		return userPref.c00.transition(userPref.c50, CALC_PERC(MID_POINT, LOW_POINT));

	// 0 -> low.
	if (gradient)
		return userPref.off.transition(userPref.c00, CALC_PERC(LOW_POINT, 1));

	return userPref.c00;
}

void AudioActor::drawConfig() {
	cout <<
		"mode: "        << mode2str(userPref.mode) << endl <<
		"channel: "     << channel2str(userPref.channel) << endl <<
		"Colors: Off: " << userPref.off.getName() <<
		", Low: "       << userPref.c00.getName() <<
		", Mid: "       << userPref.c50.getName() <<
		", High: "      << userPref.c75.getName() << endl;
	Actor::drawConfig();
	Direction::drawConfig();
	cout << SEPARATOR << endl;
}

AudioActor::Modes AudioActor::str2mode(const string& mode) {
	if (mode == "Levels")
		return Modes::Levels;
	if (mode == "VuMeter")
		return Modes::VuMeter;
	if (mode == "Single")
		return Modes::Single;
	if (mode == "Wave")
		return Modes::Wave;
	LogError("Invalid mode " + mode + " assuming Single");
	return Modes::Single;
}

string AudioActor::mode2str(Modes mode) {
	switch (mode) {
	case Modes::Levels:
		return "Levels";
	case Modes::VuMeter:
		return "VuMeter";
	case Modes::Single:
		return "Single";
	case Modes::Wave:
		return "Wave";
	}
	return "";
}

AudioActor::Channels AudioActor::str2channel(const string& channel) {
	if (channel == "Both")
		return Channels::Both;
	if (channel == "Right")
		return Channels::Right;
	if (channel == "Left")
		return Channels::Left;
	if (channel == "Mono")
		return Channels::Mono;
	LogError("Invalid mode " + channel + " assuming Both");
	return Channels::Both;
}

string AudioActor::channel2str(Channels channel) {
	switch (channel) {
	case Channels::Both:
		return "Both";
	case Channels::Left:
		return "Left";
	case Channels::Right:
		return "Right";
	case Channels::Mono:
		return "Mono";
	}
	return "";
}
