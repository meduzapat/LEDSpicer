/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      AudioActor.cpp
 * @since     Oct 6, 2020
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2018 - 2026 Patricio A. Rossi (MeduZa)
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

AudioActor::Values AudioActor::value;

AudioActor::AudioActor(StringUMap& parameters, Group* const group) :
	Actor(parameters, group, REQUIRED_PARAM_ACTOR_AUDIO),
	Direction(parameters.exists("direction") ? parameters["direction"] : "Forward"),
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
			static_cast<uint16_t>(group->size() / CHANNELS),
			static_cast<uint16_t>((group->size() / CHANNELS) + group->size() % CHANNELS)
		};
	else
		totalElements = {
			group->size(),
			group->size()
		};

	switch (userPref.mode) {
	case Modes::VuMeter:
		if (group->size() < VU_MIN_ELEMENTS)
			throw Error("To use VU meter a minimum of ")
				<< VU_MIN_ELEMENTS
				<< " elements is needed on the group";
		break;
	case Modes::Disco:
	case Modes::Wave:
		colorData.insert(colorData.begin(), group->size(), Color::Off);
		break;
	default: break;
	}

	// No bouncing here.
	bounce = false;
#ifdef DEVELOP
	if (Log::isLogging(LOG_DEBUG)) {
		cout <<
			"Total Elements:"  << group->size() <<
			"Total Left:"  << totalElements.l <<
			"Total Right:" << totalElements.r << endl;
	}
#endif
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
	value.l = value.r = 0;
	calcPeak();
}

#ifdef DEVELOP
void AudioActor::displayPeak() {
	if (Log::isLogging(LOG_DEBUG)) {
		cout <<
			"L:"  << std::left    << std::setfill(' ')  << std::setw(5) << to_string(value.l) <<
			" R:" << std::setw(5) << to_string(value.r) << endl;
	}
}
#endif

void AudioActor::single() {

	refreshPeak();

	if (not value.l and not value.r) return;

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
		for (uint16_t c = 0; c < totalElements.l; ++c) {
			changeElementColor(
				direction == Directions::Forward ? c : totalElements.l - c - 1,
				l,
				filter
			);
		}
	}
	if (userPref.channel & Channels::Right) {
		for (uint16_t c = 0; c < totalElements.r; ++c) {
			changeElementColor(
				direction == Directions::Forward ? getNumberOfElements() - c - 1 : totalElements.r + c,
				r,
				filter
			);
		}
	}
}

void AudioActor::vuMeters() {

	/**
	 * Function to draw the elements.
	 * @param elems the number of elements to draw.
	 * @param start the starting point.
	 * @param total the total number of elements.
	 * @param reverse if set will draws backward.
	 */
	auto tintFn = [&] (const uint16_t elems, const uint16_t start, const uint16_t total, const bool reverse) {
		for (uint16_t e = 0; e < elems; ++e) {
			uint16_t s(reverse ? start - e : start + e);
			changeElementColor(s, detectColor(round((e + 1) * 100.00f / total), false), filter);
		}
	};

	refreshPeak();
	uint16_t val;
	// TotalElements l or r have the total number of elements for solo channels.
	// Convert to mono and them to elements.
	if (userPref.channel == Channels::Mono) {
		val = totalElements.l * ((value.l + value.r) / CHANNELS) / 100.f;
		if (not val) return;
		if (val > totalElements.l) val = totalElements.l;

		if (direction == Directions::Forward)
			// left to right
			tintFn(val, 0, totalElements.l, false);
		else
			// right to left
			tintFn(val, totalElements.l - 1, totalElements.l, true);
		return;
	}

	// Convert left channel peak to elements.
	if (userPref.channel & Channels::Left) {
		val  = totalElements.l * value.l / 100.f;
		if (val > totalElements.l) val = totalElements.l;
		if (val) {
			if (direction == Directions::Forward)
				// left to right (half way)
				tintFn(val, 0, totalElements.l, false);
			else
				// right (half way) to left
				tintFn(val, totalElements.l -1, totalElements.l, true);
		}
	}

	// Convert right channel peak to elements.
	if (userPref.channel & Channels::Right) {
		val = totalElements.r * value.r / 100.f;
		if (val > totalElements.r) val = totalElements.r;
		if (val) {
			if (direction == Directions::Forward)
				// right to left (half way)
				tintFn(val, getNumberOfElements() - 1, totalElements.r, true);
			else
				// left (half way) + 1 to right (using left channel size as mark)
				tintFn(val, (userPref.channel == Channels::Both ? totalElements.l : 0), totalElements.r, false);
		}
	}
}

void AudioActor::waves() {

	uint16_t
		t = colorData.size() - 1,
		h = totalElements.l - 1;
	// single/mono VS stereo
	if (userPref.channel == Channels::Both) {
		// outside in
		if (direction == Directions::Forward) {
			for (uint16_t c = 0; c < h; c++) {
				colorData[h - c] = colorData[h - c - 1];
				colorData[h + c + 1] = colorData[h + c + 2];
			}
			if (totalElements.l != totalElements.r)
				colorData[t - 1] = colorData[t];
		}
		// inside out
		else {
			for (uint16_t c = 0; c < h; c++) {
				colorData[c] = colorData[c + 1];
				colorData[t - c] = colorData[t - c - 1];
			}
			if (totalElements.l != totalElements.r)
				colorData[h + 2] = colorData[h + 1];
		}
	}
	else {
		for (uint16_t c = 0; c < t; c++)
			if (direction == Directions::Forward)
				colorData[t - c] = colorData[t - c - 1];
			else
				colorData[c] = colorData[c + 1];
	}

	refreshPeak();

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

	for (uint16_t c = 0; c < colorData.size(); ++c)
		changeElementColor(c, colorData[c], filter);
}

void AudioActor::disco() {

	// Fade all existing colors to create persistence.
	for (auto& value : colorData) {
		auto tone(value.getMonochrome());
		if (not tone) continue;
		value.set(value.fade(80));
	}

	refreshPeak();

	auto applySmoothing = [&](uint16_t idx, uint16_t totalSize, const Color& mainColor) {
		// Apply main color to the primary index.
		colorData[idx] = mainColor.transition(colorData[idx], 50);

		// Smooth to adjacent elements if within bounds.
		if (idx > 0)
			colorData[idx - 1] = userPref.c50.transition(colorData[idx - 1], 50);
		if (idx < totalSize - 1)
			colorData[idx + 1] = userPref.c50.transition(colorData[idx + 1], 50);
	};

	auto calculateAndApply = [&](uint16_t val, uint16_t totalSize, bool isLeft) {
		if (not val) return;

		// Map value (0-100) to index (0 to totalSize-1).
		uint16_t idx = static_cast<uint16_t>(round(static_cast<float>(val) / 100.0f * (totalSize - 1)));

		// Adjust index based on direction and channel.
		switch (userPref.channel) {
		case Channels::Mono:
		case Channels::Left:
			idx = (direction == Directions::Forward ? idx : totalSize - 1 - idx);
			break;
		case Channels::Right:
			idx = (direction == Directions::Forward ? totalSize - 1 - idx : idx);
			break;
		case Channels::Both:
			if (isLeft) {
				idx = (direction == Directions::Forward ? idx : totalElements.l - 1 - idx);
				break;
			}
			// Right channel offset.
			idx += totalElements.l;
			idx = (direction == Directions::Forward ? getNumberOfElements() - 1 - idx + totalElements.l : idx);
			break;
		}

#ifdef DEVELOP
		if (Log::isLogging(LOG_DEBUG)) {
			cout <<
				(isLeft ? "L" : "R")    <<
				" Val:" << std::setw(3) << val <<
				" Idx:" << std::setw(3) << idx <<
				"/"     << totalSize    << endl;
		}
#endif

		// Detect main color based on value (using gradient for smoothness).
		Color mainColor = detectColor(val);

		// Apply with smoothing.
		applySmoothing(idx, totalSize, mainColor);
	};

	if (userPref.channel == Channels::Mono) {
		calculateAndApply((value.l + value.r) / 2, totalElements.l, false);
	}
	else {
		if (userPref.channel & Channels::Left)
			calculateAndApply(value.l, totalElements.l, true);
		if (userPref.channel & Channels::Right)
			calculateAndApply(value.r, totalElements.r, false);
	}

	// Apply colors to the elements.
	for (uint16_t c = 0; c < getNumberOfElements(); ++c) {
		changeElementColor(c, colorData[c], filter);
	}
}

#define CALC_PERC(t, b) round(abs(percent - (b)) * 100.00 / ((t) - (b)))

Color AudioActor::detectColor(uint8_t percent, bool gradient) {

	// Off.
	if (not percent) return userPref.off;

	if (gradient) {
		if (percent == 100)
			return userPref.c75;
	}
	else if (percent > HIG_POINT) {
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

Color AudioActor::getColorByPercent(const uint8_t percent) {

	// High.
	if (percent > HIG_POINT) return userPref.c75;
	// Mid -> High.
	if (percent > MID_POINT) return userPref.c50;
	// Low -> Mid.
	if (percent > LOW_POINT) return userPref.c00;
	// Low.
	return userPref.off;
}

void AudioActor::restart() {
	value.l = value.r = 0;
	Actor::restart();
}

void AudioActor::drawConfig() const {
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
	if (mode == "VuMeter") return Modes::VuMeter;
	if (mode == "Single")  return Modes::Single;
	if (mode == "Wave")    return Modes::Wave;
	if (mode == "Disco")   return Modes::Disco;
	LogError("Invalid mode " + mode + " assuming Single");
	return Modes::Single;
}

string AudioActor::mode2str(Modes mode) {
	switch (mode) {
	case Modes::VuMeter: return "VuMeter";
	case Modes::Single:  return "Single";
	case Modes::Wave:    return "Wave";
	case Modes::Disco:   return "Disco";
	}
	return "";
}

AudioActor::Channels AudioActor::str2channel(const string& channel) {
	if (channel == "Both")  return Channels::Both;
	if (channel == "Right") return Channels::Right;
	if (channel == "Left")  return Channels::Left;
	if (channel == "Mono")  return Channels::Mono;
	LogError("Invalid mode " + channel + " assuming Both");
	return Channels::Both;
}

string AudioActor::channel2str(Channels channel) {
	switch (channel) {
	case Channels::Both:  return "Both";
	case Channels::Left:  return "Left";
	case Channels::Right: return "Right";
	case Channels::Mono:  return "Mono";
	}
	return "";
}

uint16_t AudioActor::getFullFrames() const {
	return 0;
}

float AudioActor::getRunTime() const {
	if (secondsToEnd) return secondsToEnd;
	return 0;
}
