/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      PulseAudio.cpp
 * @since     Dec 13, 2018
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2018 - 2020 Patricio A. Rossi (MeduZa)
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

#include "PulseAudio.hpp"

using namespace LEDSpicer::Animations;

pa_threaded_mainloop* PulseAudio::tml = nullptr;
pa_context* PulseAudio::context = nullptr;
pa_stream* PulseAudio::stream = nullptr;
uint8_t
	PulseAudio::instances     = 0,
	PulseAudio::totalChannels = 0;
string PulseAudio::source;
uint16_t PulseAudio::peak = MIN_MAX_PEAK;
std::mutex PulseAudio::mutex;

vector<int16_t> PulseAudio::rawData;

PulseAudio::PulseAudio(umap<string, string>& parameters, Group* const group) :
	Actor(parameters, group, REQUIRED_PARAM_ACTOR_PULSEAUDIO),
	Direction(parameters.count("direction") ? parameters["direction"] : "Forward"),
	userPref({
		Color::getColor(parameters["off"]),
		Color::getColor(parameters["low"]),
		Color::getColor(parameters["mid"]),
		Color::getColor(parameters["high"]),
		str2mode(parameters["mode"]),
		str2channel(parameters["channel"])
	}),
	total(userPref.channel != Channels::Both ? group->size() : group->size() / 2)
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

	++instances;
	peak = MIN_MAX_PEAK;

	LogInfo("Connecting to pulseaudio");

	// If not connected to PA do it.
	if (tml) {
		LogInfo("Reusing Pulseaudio connection");
		return;
	}

	tml = pa_threaded_mainloop_new();
	if (not tml)
		throw Error("Failed create pulseaudio main loop");

	context = pa_context_new(pa_threaded_mainloop_get_api(tml), PACKAGE_STRING);
	if (not context)
		throw Error("Failed to create pulseaudio context");
	pa_context_set_state_callback(context, PulseAudio::onContextSetState, &userPref);
	pa_context_connect(context, nullptr, PA_CONTEXT_NOFLAGS, nullptr);
	pa_threaded_mainloop_start(tml);
	LogDebug("Pulseaudio connected");
}

PulseAudio::~PulseAudio() {
	std::lock_guard<std::mutex> lock(mutex);
	--instances;
	if (instances == 0)
		disconnect();
}

void PulseAudio::disconnect() {

	LogInfo("Closing pulseaudio connection");

	pa_signal_done();
	pa_threaded_mainloop_stop(tml);

	if (stream) {
		pa_stream_disconnect(stream);
		pa_stream_unref(stream);
		stream = nullptr;
	}

	if (context) {
		pa_context_disconnect(context);
		pa_context_unref(context);
		context = nullptr;
	}

	if (tml) {
		pa_threaded_mainloop_free(tml);
		tml = nullptr;
	}
}

void PulseAudio::drawConfig() {
	char cmt[PA_CHANNEL_MAP_SNPRINT_MAX], sst[PA_SAMPLE_SPEC_SNPRINT_MAX];
	cout <<
		"Type: Pulseaudio" << endl <<
		"mode: "        << mode2str(userPref.mode) << endl <<
		"channel: "     << channel2str(userPref.channel) << endl <<
		"Colors: Off: " << userPref.off.getName() <<
		", Low: " << userPref.c00.getName() <<
		", Mid: "       << userPref.c50.getName() <<
		", High: "      << userPref.c75.getName() << endl;
	Actor::drawConfig();
	Direction::drawConfig();
	cout << SEPARATOR << endl;
}

PulseAudio::Modes PulseAudio::str2mode(const string& mode) {
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

string PulseAudio::mode2str(Modes mode) {
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

PulseAudio::Channels PulseAudio::str2channel(const string& channel) {
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

string PulseAudio::channel2str(Channels channel) {
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

void PulseAudio::onContextSetState(pa_context* context, void* channels) {

	switch (pa_context_get_state(context)) {
	case PA_CONTEXT_CONNECTING:
#ifdef DEVELOP
		LogDebug("Connecting context.");
		break;
#endif
	case PA_CONTEXT_AUTHORIZING:
	case PA_CONTEXT_SETTING_NAME:
		break;

	case PA_CONTEXT_READY:
		LogDebug("Connection to pulseaudio context established");
		pa_operation_unref(pa_context_get_server_info(
			context,
			[] (pa_context* context, const pa_server_info* info, void* userdata) {
				source = info->default_sink_name;
				pa_operation_unref(pa_context_get_sink_info_list(context, onSinkInfo, userdata));
			},
			nullptr
		));
		pa_context_set_subscribe_callback(
			context,
			[] (pa_context* context, pa_subscription_event_type_t type, uint32_t idx, void* userdata) {
				if (type == PA_SUBSCRIPTION_EVENT_CHANGE) {
					LogDebug("context event changed");
					peak = MIN_MAX_PEAK;
				}
			},
			nullptr
		);
		pa_context_subscribe(context, PA_SUBSCRIPTION_MASK_SINK, nullptr, nullptr);
		break;

	case PA_CONTEXT_TERMINATED:
		LogDebug("Pulseaudio context terminated");
		break;

	case PA_CONTEXT_FAILED:
	default:
		LogError("Pulseaudio Context failed: " + string(pa_strerror(pa_context_errno(context))));
		throw;
	}
}

void PulseAudio::onSinkInfo(pa_context* condex, const pa_sink_info* info, int eol, void* userPref) {

	if (not info)
		return;

	if (source.empty())
		return;

#ifdef DEVELOP
	LogDebug("Finding monitor for " + source);
#endif

	if (source.compare(info->name) != 0) {
#ifdef DEVELOP
	LogDebug("Not found");
#endif
		return;
	}
	source = "";

#ifdef DEVELOP
	LogDebug("Found: "       + string(info->monitor_source_name));
	LogDebug("Index: "       + to_string(info->index));
	LogDebug("Description: " + string(info->description));
	char cmt[PA_CHANNEL_MAP_SNPRINT_MAX], sst[PA_SAMPLE_SPEC_SNPRINT_MAX];
	LogDebug("Sample: "      + string(pa_sample_spec_snprint(sst, sizeof(sst), &info->sample_spec)));
	LogDebug("Channels: "    + string(pa_channel_map_snprint(cmt, sizeof(cmt), &info->channel_map)));
	LogDebug("Total bytes: " + to_string(pa_frame_size(&info->sample_spec)));
	LogDebug("Sample Size: " + to_string(pa_sample_size(&info->sample_spec)));
#endif

	totalChannels = info->channel_map.channels > 1 ? 2 : 1;

	pa_sample_spec ss = {SAMPLE_FORMAT, RATE, info->sample_spec.channels};

	if (not (stream = pa_stream_new(context, STREAM_NAME, &ss, &info->channel_map))) {
		LogError("Failed to create stream: " + string(pa_strerror(pa_context_errno(context))));
		throw;
	}

	pa_stream_set_state_callback(stream, onStreamSetState, userPref);

#ifdef DEVELOP
	// underflows
	pa_stream_set_underflow_callback(
		stream,
		[](pa_stream* stream, void* userdata) {
			LogDebug("Underflow");
			peak = MIN_MAX_PEAK;
		},
		nullptr
	);
	// overflows
	pa_stream_set_overflow_callback(
		stream,
		[](pa_stream* stream, void* userdata) {
			LogDebug("overflow");
			peak = MIN_MAX_PEAK;
		},
		nullptr
	);
#endif

	pa_buffer_attr ba;
	ba.maxlength = (uint32_t) -1;
	ba.fragsize  = (uint32_t) -1;
	if (pa_stream_connect_record(stream, info->monitor_source_name, &ba, PA_STREAM_PEAK_DETECT) < 0) {
		LogError("Failed to connect to output stream: " + string(pa_strerror(pa_context_errno(context))))
		throw;
	}
}

void PulseAudio::onStreamSetState(pa_stream* stream, void* userPref) {

	switch (pa_stream_get_state(stream)) {

#ifdef DEVELOP
	case PA_STREAM_CREATING:
		LogDebug("Creating stream.");
		break;

	case PA_STREAM_TERMINATED:
		LogDebug("Stream terminated.");
		break;
#endif

	case PA_STREAM_READY:
		LogInfo("Stream successfully created.");
		pa_stream_set_read_callback(stream, onStreamRead, userPref);
		break;

	case PA_STREAM_FAILED:
		LogError("Connection to stream failed: " + string(pa_strerror(pa_context_errno(context))));
		throw;
	}
}

void PulseAudio::onStreamRead(pa_stream* stream, size_t length, void* userdata) {

	rawData.clear();
	if (not length)
		return;

	// Read peaks.
	const void *data;
	if (pa_stream_peek(stream, &data, &length) < 0) {
		LogError("Stream peek failed: " + string(pa_strerror(pa_context_errno(context))));
		return;
	}

	// Hole detected (no idea what a hole is but is on the docs).
	if (not data and length > 0) {
		pa_stream_drop(stream);
		return;
	}

	// Parse data.
	std::lock_guard<std::mutex> lock(mutex);
	const int16_t* buffer = static_cast<const int16_t*>(data);
	PulseAudio::rawData = std::move(vector<int16_t>(buffer, buffer + (length / 2)));
	pa_stream_drop(stream);
}

void PulseAudio::calculateElements() {
	std::lock_guard<std::mutex> lock(mutex);
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

PulseAudio::Values PulseAudio::getPeak() {
	Values vals;
	for (size_t c = 0; c < rawData.size(); ++c) {
		uint16_t v = abs(rawData[c]);
		// Even for left, odd for right.
		if (c % 2) {
			if (v > vals.r)
				vals.r = v;
		}
		else {
			if (v > vals.l)
				vals.l = v;
		}
	}
	setPeak(vals);
	return vals;
}

void PulseAudio::setPeak(const Values& peaks) {
	if (peaks.r > peak)
		peak = peaks.r;
	if (peaks.l > peak)
		peak = peaks.l;
/*	uint16_t m = (peak + peaks.l + peaks.r) / (totalChannels + 1);
	if (m > peak)
		peak = m;*/
	if (peak < MIN_MAX_PEAK)
		peak = MIN_MAX_PEAK;
#ifdef DEVELOP
	cout
		<< "L:" << std::left << std::setfill(' ') << std::setw(5) << to_string(peaks.l)
		<< " R:" << std::setw(5) << to_string(peaks.r)
		<< " Peak:" << std::setw(5) << to_string(peak) << endl;
#endif
}

#define TO_PERC(x) round((x) * 100.00 / peak)

void PulseAudio::vuMeters() {

	/*
	 * Function to draw the elements.
	 */
	auto tintFn = [&] (uint8_t elems, uint8_t start, bool reverse) {
		for (uint8_t e = 0; e <= elems; ++e) {
			uint8_t s = reverse ? start - e : start + e;
#ifdef DEVELOP
			cout << std::setw(3) << to_string(s);
#else
			changeElementColor(s, detectColor(round((e + 1) * 100.00 / total), false), filter);
#endif
		}
	};

	auto singleData = getPeak();
	if (not singleData.l and not singleData.r)
		return;

	uint16_t val;
	// Convert to mono and them to elements.
	if (userPref.channel == Channels::Mono) {
		val = total * TO_PERC((singleData.l + singleData.r) / 2) / 100;
		if (not val)
			return;
		if (val > total)
			val = total;

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
		val  = total * TO_PERC(singleData.l) / 100;
		if (val > total)
			val = total;
		if (val) {
			--val;
			if (val) {
				if (direction == Directions::Forward)
					tintFn(val, 0, false);
				else
					tintFn(val, total, true);
			}
		}
	}

	// Convert right channel peak to elements.
	if (userPref.channel & Channels::Right) {
		val = total * TO_PERC(singleData.r) / 100;
		if (val > total)
			val = total;
		if (val) {
			--val;
			if (val) {
				if (direction == Directions::Forward)
					tintFn(val, getNumberOfElements() - 1, true);
				else
					tintFn(val, total, false);
			}
		}
	}
}

void PulseAudio::levels() {

	Values vals;
	if (rawData.empty()) {
		setPeak(vals);
		return;
	}
	uint8_t e = userPref.channel == Channels::Both ? total : 0;
	for (uint8_t c = 0, c2 = 0; c < total; ++c) {
		uint16_t
			vR = rawData.size() > c2 ? abs(rawData[c2++]) : 0,
			vL = rawData.size() > c2 ? abs(rawData[c2++]) : 0;
		if (vR > peak)
			peak = vR;
		if (vL > peak)
			peak = vL;
		if (vR > vals.r)
			vals.r = vR;
		if (vL > vals.l)
			vals.l = vL;
		if (userPref.channel & Channels::Mono)
			changeElementColor(c, detectColor(TO_PERC((vL + vR) / 2)), filter);
		if (userPref.channel & Channels::Left)
			changeElementColor(c, detectColor(TO_PERC(vL)), filter);
		if (userPref.channel & Channels::Right)
			changeElementColor(e + c, detectColor(TO_PERC(vR)), filter);
	}

	setPeak(vals);
}

void PulseAudio::waves() {

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
	auto singleData = getPeak();

	switch (userPref.channel) {
	case Channels::Both:
		if (direction == Directions::Forward) {
			waveData[0] = detectColor(TO_PERC(singleData.l));
			waveData[t] = detectColor(TO_PERC(singleData.r));
		}
		else {
			waveData[h] = detectColor(TO_PERC(singleData.l));
			waveData[h + 1] = detectColor(TO_PERC(singleData.r));
		}
		break;
	case Channels::Mono:
		waveData[direction == Directions::Forward ? 0 : t] = detectColor(TO_PERC(singleData.r + singleData.l / 2));
		break;
	case Channels::Left:
		waveData[direction == Directions::Forward ? 0 : t] = detectColor(TO_PERC(singleData.l));
		break;
	case Channels::Right:
		waveData[direction == Directions::Forward ? 0 : t] = detectColor(TO_PERC(singleData.r));
		break;
	}

	for (uint8_t c = 0; c < waveData.size(); ++c) {
		changeElementColor(c, waveData[c], filter);
#ifdef DEVELOP
	cout << "C:" << to_string(c) << " To: ";
	waveData[c].drawHex();
	cout << endl;
#endif
	}
}

void PulseAudio::single() {
	auto singleData = getPeak();
	uint8_t e = userPref.channel == Channels::Both ? total : 0;
	for (uint8_t c = 0; c < total; ++c) {
		if (userPref.channel & Channels::Mono)
			changeElementColor(c, detectColor(TO_PERC((singleData.l + singleData.r) / 2)), filter);
		if (userPref.channel & Channels::Left)
			changeElementColor(c, detectColor(TO_PERC(singleData.l)), filter);
		if (userPref.channel & Channels::Right)
			changeElementColor(e + c, detectColor(TO_PERC(singleData.r)), filter);
	}
}

LEDSpicer::Color PulseAudio::detectColor(uint8_t percent, bool gradient) {
	#define CALC_PERC(t, b) round(abs(percent - (b)) * 100.00 / ((t) - (b)))

#ifdef DEVELOP
	cout << "Percent: " <<  to_string(percent) << endl;
#endif

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
