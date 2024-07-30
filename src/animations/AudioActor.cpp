/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      AudioActor.cpp
 * @since     Oct 6, 2020
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2018 - 2024 Patricio A. Rossi (MeduZa)
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
	})
{
	if (userPref.channel == Channels::Both)
		totalElements = {
			static_cast<uint8_t>(group->size() / CHANNELS),
			static_cast<uint8_t>((group->size() / CHANNELS) + group->size() % CHANNELS)
		};
	else
		totalElements = {
			group->size(),
			group->size()
		};

	switch (userPref.mode) {
	case Modes::VuMeter:
		if (group->size() < VU_MIN_ELEMETS)
			throw Error("To use VU meter a minimum of " + to_string(VU_MIN_ELEMETS) + " elements is needed on the group.");
		break;
	case Modes::Disco:
	case Modes::Wave:
		colorData.insert(colorData.begin(), group->size(), Color());
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
	case Modes::Wave:
		waves();
		break;
	case Modes::Disco:
		disco();
		break;
	}
}

void AudioActor::refreshPeak() {
#ifdef DEVELOP
	displayPeak();
#endif
	if (frame == lastF)
		return;
	lastF = frame;
	value.l = value.r = 0;
	calcPeak();
}

#ifdef DEVELOP
void AudioActor::displayPeak() {
	cout
		<< "L:" << std::left << std::setfill(' ') << std::setw(5) << to_string(value.l)
		<< " R:" << std::setw(5) << to_string(value.r) << endl;
}
#endif

void AudioActor::single() {

	refreshPeak();

	if (not value.l and not value.r)
		return;

	// Left & Right.
	Color l, r;

	if (userPref.channel == Channels::Mono) {
		l = detectColor((value.l + value.r) / CHANNELS);
	}
	else {
		if (userPref.channel & Channels::Left)
			l = detectColor(value.l);
		if (userPref.channel & Channels::Right)
			r = detectColor(value.r);
	}
	// Both or Left follows the same logic.
	if ((userPref.channel & 5)) {
		for (uint8_t c = 0; c < totalElements.l; ++c) {
			changeElementColor(
				direction == Directions::Forward ? c : totalElements.l - c - 1,
				l,
				filter
			);
		}
	}
	if (userPref.channel & Channels::Right) {
		for (uint8_t c = 0; c < totalElements.r; ++c) {
			changeElementColor(
				direction == Directions::Forward ? getNumberOfElements() - c - 1 : totalElements.l + c,
				r,
				filter
			);
		}
	}
}

void AudioActor::vuMeters() {

	/**
	 * Function to draw the elements.
	 * @param uint8_t ele-ms the number of elements to draw.
	 * @param uint8_t start the starting point.
	 * @param uint8_t total the total number of elements.
	 * @param bool reverse if set will draws backward.
	 */
	auto tintFn = [&] (const uint8_t elems, const uint8_t start, const uint8_t total, const bool reverse) {
		for (uint8_t e = 0; e < elems; ++e) {
			uint8_t s(reverse ? start - e : start + e);
#ifdef DEVELOP
			cout << std::setw(3) << to_string(s);
#endif
			changeElementColor(s, detectColor(round((e + 1) * 100.00f / total), false), filter);
		}
	};

	refreshPeak();
	uint8_t val;

	/*
	 * Forward / Backward, Start / Total table, for Left, Righ, Mono and Both.
	 *     L  R  M  BL  BR
	 * FS: 0  F- 0  0   F-
	 * BS: F- 0  F- HL- HR-
	 * FT: F- F- F- HL- HR-
	 * BT: F- F- F- HL- HR-
	 * Full/Half size
	 * -1
	 */

	// TotalElements l or r have the total number of elements for solo channels.
	// Convert to mono and them to elements.
	if (userPref.channel == Channels::Mono) {
		val = totalElements.l * ((value.l + value.r) / CHANNELS) / 100.00f;
		if (not val)
			return;
		if (val > totalElements.l)
			val = totalElements.l;

		if (direction == Directions::Forward)
			tintFn(val, 0, totalElements.l, false);
		else
			tintFn(val, totalElements.l - 1, totalElements.l, true);
		return;
	}

	// Convert left channel peak to elements.
	if (userPref.channel & Channels::Left) {
		val  = totalElements.l * value.l / 100;
		if (val > totalElements.l)
			val = totalElements.l;
		if (val) {
			if (direction == Directions::Forward)
				tintFn(val, 0, totalElements.l, false);
			else
				tintFn(val, totalElements.l -1, totalElements.l, true);
		}
	}

	// Convert right channel peak to elements.
	if (userPref.channel & Channels::Right) {
		val = totalElements.r * value.r / 100;
		if (val > totalElements.r)
			val = totalElements.r;
		if (val) {
			uint8_t start, total;
			if (direction == Directions::Forward)
				tintFn(
					val,
					getNumberOfElements() - 1,
					totalElements.r,
					true
				);
			else
				tintFn(
					val,
					(userPref.channel == Channels::Both ? totalElements.r -1 : 0),
					totalElements.r,
					false
				);
		}
	}
}

void AudioActor::waves() {

	uint8_t
		t = colorData.size() - 1,
		h = totalElements.l - 1;
	// single/mono VS stereo
	if (userPref.channel == Channels::Both) {
		// outside in
		if (direction == Directions::Forward) {
			for (uint8_t c = 0; c < h; c++) {
				colorData[h - c] = colorData[h - c - 1];
				colorData[h + c + 1] = colorData[h + c + 2];
			}
			if (totalElements.l != totalElements.r)
				colorData[t - 1] = colorData[t];
		}
		// inside out
		else {
			for (uint8_t c = 0; c < h; c++) {
				colorData[c] = colorData[c + 1];
				colorData[t - c] = colorData[t - c - 1];
			}
			if (totalElements.l != totalElements.r)
				colorData[h + 2] = colorData[h + 1];
		}
	}
	else {
		for (uint8_t c = 0; c < t; c++)
			if (direction == Directions::Forward)
				colorData[t - c] = colorData[t - c - 1];
			else
				colorData[c] = colorData[c + 1];
	}

	refreshPeak();
#ifdef DEVELOP
	displayPeak();
#endif

	switch (userPref.channel) {
	case Channels::Both:
		if (direction == Directions::Forward) {
			colorData[0] = detectColor(value.l);
			colorData[t] = detectColor(value.r);
		}
		else {
			colorData[h] = detectColor(value.l);
			colorData[h + 1] = detectColor(value.r);
		}
		break;
	case Channels::Mono:
		colorData[direction == Directions::Forward ? 0 : t] = detectColor(value.r + value.l / 2);
		break;
	case Channels::Left:
		colorData[direction == Directions::Forward ? 0 : t] = detectColor(value.l);
		break;
	case Channels::Right:
		colorData[direction == Directions::Forward ? 0 : t] = detectColor(value.r);
		break;
	}

	for (uint8_t c = 0; c < colorData.size(); ++c)
		changeElementColor(c, colorData[c], filter);
}

void AudioActor::disco() {
	for (auto& value : colorData) {
		auto tone(value.getMonochrome());
		if (not tone)
			continue;
		value.set(value.fade(80));
	}
	refreshPeak();

	auto calculateIndex = [&](uint8_t val, uint8_t totalSize, bool isLeft) {
		float
			per = 0,
			// Individual size.
			boxSize = 100 / (totalSize - 1);
		uint8_t
			idx   = val / boxSize,
			start = idx * boxSize,
			end   = (idx + 1) * boxSize;
		if (end > 100)
			end = 100;
		per = static_cast<float>(val - start) / (end - start + 1.00f) * 100.00f;
		if (idx > 0 and per < 1) {
			--idx;
			per = 100;
		}
		switch (userPref.channel) {
		case Channels::Mono:
		case Channels::Left:
			idx = (direction == Directions::Forward ? idx : totalSize - idx);
			break;
		case Channels::Right:
			idx = (direction == Directions::Forward ? totalSize - idx : idx);
			break;
		case Channels::Both:
			// Forware outside in, Backward inside out.
			if (isLeft) {
				idx = (direction == Directions::Forward ? idx : totalElements.l - 1 - idx);
				break;
			}
			idx = (direction == Directions::Forward ? getNumberOfElements() - 1 - idx : totalElements.r + idx - (totalElements.r - totalElements.l));
			break;
		}
#ifdef DEVELOP
		cout << "Val:"  << std::setw(3) << to_string(val)
		     << " Idx:"  << std::setw(3) << to_string(idx)
		     << "/" << to_string(totalSize)
		     << " Start:"  << std::setw(3) << to_string(start)
		     << " End:"  << std::setw(3) << to_string(end)
		     << " Per:"  << std::setw(3) << to_string(static_cast<uint8_t>(per)) << endl;
#endif
		colorData[idx] = detectColor(per);
	};

	if (userPref.channel == Channels::Mono) {
		calculateIndex((value.l + value.r) / 2, totalElements.l, false);
	}
	else {
		if (userPref.channel & Channels::Left)
			calculateIndex(value.l, totalElements.l, true);
		if (userPref.channel & Channels::Right)
			calculateIndex(value.r, totalElements.r, false);
	}
	// Apply colors to the elements
	for (uint8_t c = 0; c < getNumberOfElements(); ++c) {
		changeElementColor(c, colorData[c], filter);
	}
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

LEDSpicer::Color AudioActor::getColorByPercent(const uint8_t percent) {

	// High.
	if (percent > HIG_POINT)
		return userPref.c75;
	// Mid -> High.
	if (percent > MID_POINT)
		return userPref.c50;
	// Low -> Mid.
	if (percent > LOW_POINT)
		return userPref.c00;
	// Low.
	return userPref.off;
}

void AudioActor::restart() {
	value.l = value.r = 0;
	Actor::restart();
}

void AudioActor::drawConfig() {
	cout <<
		"mode: "        << mode2str(userPref.mode)       << endl <<
		"channel: "     << channel2str(userPref.channel) << endl <<
		"Colors: Off: " << userPref.off.getName()        <<
		", Low: "       << userPref.c00.getName()        <<
		", Mid: "       << userPref.c50.getName()        <<
		", High: "      << userPref.c75.getName()        << endl <<
		"Direction: "   << (direction == Direction::Directions::Forward ? "Inward" : "Outward") << endl;
	Actor::drawConfig();
}

AudioActor::Modes AudioActor::str2mode(const string& mode) {
	if (mode == "VuMeter")
		return Modes::VuMeter;
	if (mode == "Single")
		return Modes::Single;
	if (mode == "Wave")
		return Modes::Wave;
	if (mode == "Disco")
		return Modes::Disco;
	LogError("Invalid mode " + mode + " assuming Single");
	return Modes::Single;
}

string AudioActor::mode2str(Modes mode) {
	switch (mode) {
	case Modes::VuMeter:
		return "VuMeter";
	case Modes::Single:
		return "Single";
	case Modes::Wave:
		return "Wave";
	case Modes::Disco:
		return "Disco";
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

const uint16_t AudioActor::getFullFrames() const {
	return 0;
}

const float AudioActor::getRunTime() const {
	if (secondsToEnd) {
		return secondsToEnd;
	}
	return 0;
}
