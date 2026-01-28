/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      PulseAudio.cpp
 * @since     Dec 13, 2018
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

#include "PulseAudio.hpp"

using namespace LEDSpicer::Animations;

pa_threaded_mainloop* PulseAudio::tml = nullptr;
pa_context* PulseAudio::context       = nullptr;
pa_stream* PulseAudio::stream         = nullptr;
uint8_t PulseAudio::instances         = 0;
string PulseAudio::source;
std::mutex PulseAudio::mutex;
std::atomic<PulseAudio::State> PulseAudio::state{State::Disconnected};

vector<uint8_t> PulseAudio::rawData;

PulseAudio::PulseAudio(StringUMap& parameters, Group* const group) :
	AudioActor(parameters, group)
{
	++instances;

	LogInfo("Connecting to PulseAudio");

	// If already connected or connecting, reuse.
	if (state.load() != State::Disconnected) {
		LogInfo("Reusing PulseAudio connection");
		return;
	}

	connect();
}

PulseAudio::~PulseAudio() {
	std::lock_guard<std::mutex> lock(mutex);
	--instances;
	if (instances == 0) {
		// Signal reconnect thread to stop and wait for it.
		State expected = State::Reconnecting;
		if (state.compare_exchange_strong(expected, State::ShuttingDown)) {
			while (state.load() == State::ShuttingDown) {
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
			}
		}
		disconnect();
		state.store(State::Disconnected);
	}
}

void PulseAudio::connect() {

	state.store(State::Connecting);

	tml = pa_threaded_mainloop_new();
	if (not tml)
		throw Error("Failed to create PulseAudio main loop");

	context = pa_context_new(pa_threaded_mainloop_get_api(tml), PROJECT_NAME " " PROJECT_VERSION);
	if (not context) {
		pa_threaded_mainloop_free(tml);
		tml = nullptr;
		throw Error("Failed to create PulseAudio context");
	}

	pa_context_set_state_callback(context, PulseAudio::onContextSetState, nullptr);
	pa_context_connect(context, nullptr, PA_CONTEXT_NOFLAGS, nullptr);
	pa_threaded_mainloop_start(tml);
	LogDebug("PulseAudio connection initiated");
}

void PulseAudio::disconnect() {

	LogInfo("Closing PulseAudio connection");

	if (tml)
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

void PulseAudio::scheduleReconnect() {

	// Prevent multiple reconnection threads.
	State expected = State::Connecting;
	if (not state.compare_exchange_strong(expected, State::Reconnecting)) {
		// Also try from Connected state (in case of mid-operation failure).
		expected = State::Connected;
		if (not state.compare_exchange_strong(expected, State::Reconnecting))
			return;
	}

	std::thread([] {
		while (state.load() == State::Reconnecting) {
			LogInfo("Retrying PulseAudio connection in " + to_string(RECONNECT_WAIT) + " seconds...");
			std::this_thread::sleep_for(std::chrono::seconds(RECONNECT_WAIT));

			// Check if shutdown was requested.
			if (state.load() != State::Reconnecting) break;

			try {
				disconnect();
				connect();
				// connect() sets state to Connecting, stream callback will set Connected.
				return;
			}
			catch (Error& e) {
				LogWarning("PulseAudio reconnection failed: " + e.getMessage());
				state.store(State::Reconnecting);
			}
			catch (...) {
				LogWarning("PulseAudio reconnection failed");
				state.store(State::Reconnecting);
			}
		}
		state.store(State::Disconnected);
	}).detach();
}

void PulseAudio::drawConfig() const {
	cout << "Type: PulseAudio" << endl;
	AudioActor::drawConfig();
}

void PulseAudio::onContextSetState(pa_context* context, void* userdata) {

	switch (pa_context_get_state(context)) {
	case PA_CONTEXT_CONNECTING:
#ifdef DEVELOP
		LogDebug("Connecting context");
#endif
		break;

	case PA_CONTEXT_AUTHORIZING:
	case PA_CONTEXT_SETTING_NAME:
		break;

	case PA_CONTEXT_READY:
		LogDebug("Connection to PulseAudio context established");
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
#ifdef DEVELOP
					LogDebug("Context event changed");
#endif
					onSinkInfo(context, nullptr, 0, userdata);
				}
			},
			nullptr
		);
		pa_context_subscribe(context, PA_SUBSCRIPTION_MASK_SINK, nullptr, nullptr);
		break;

	case PA_CONTEXT_TERMINATED:
		LogDebug("PulseAudio context terminated");
		break;

	case PA_CONTEXT_FAILED:
	default:
		LogWarning("PulseAudio context failed: " + string(pa_strerror(pa_context_errno(context))));
		scheduleReconnect();
		break;
	}
}

void PulseAudio::onSinkInfo(pa_context* context, const pa_sink_info* info, int eol, void* userdata) {

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
	char cmt[PA_CHANNEL_MAP_SNPRINT_MAX], sst[PA_SAMPLE_SPEC_SNPRINT_MAX];
	LogDebug("Found: "       + string(info->monitor_source_name));
	LogDebug("Index: "       + to_string(info->index));
	LogDebug("Description: " + string(info->description));
	LogDebug("Sample: "      + string(pa_sample_spec_snprint(sst, sizeof(sst), &info->sample_spec)));
	LogDebug("Channels: "    + string(pa_channel_map_snprint(cmt, sizeof(cmt), &info->channel_map)));
	LogDebug("Total bytes: " + to_string(pa_frame_size(&info->sample_spec)));
	LogDebug("Sample Size: " + to_string(pa_sample_size(&info->sample_spec)));
#endif

	pa_sample_spec ss = {SAMPLE_FORMAT, RATE, CHANNELS};

	stream = pa_stream_new(context, STREAM_NAME, &ss, &info->channel_map);
	if (not stream) {
		LogWarning("Failed to create stream: " + string(pa_strerror(pa_context_errno(context))));
		scheduleReconnect();
		return;
	}

	pa_stream_set_state_callback(stream, onStreamSetState, userdata);

#ifdef DEVELOP
	pa_stream_set_underflow_callback(
		stream,
		[](pa_stream* stream, void* userdata) {
			LogDebug("Underflow");
		},
		nullptr
	);
	pa_stream_set_overflow_callback(
		stream,
		[](pa_stream* stream, void* userdata) {
			LogDebug("Overflow");
		},
		nullptr
	);
#endif

	pa_buffer_attr ba;
	ba.maxlength = -1;
	ba.fragsize  = sizeof(float) * CHANNELS;
	ba.prebuf    = 0;
	if (pa_stream_connect_record(stream, info->monitor_source_name, &ba, PA_STREAM_PEAK_DETECT) < 0) {
		LogWarning("Failed to connect to output stream: " + string(pa_strerror(pa_context_errno(context))));
		scheduleReconnect();
	}
}

void PulseAudio::onStreamSetState(pa_stream* stream, void* userdata) {

	switch (pa_stream_get_state(stream)) {

#ifdef DEVELOP
	case PA_STREAM_CREATING:
		LogDebug("Creating stream");
		break;

	case PA_STREAM_TERMINATED:
		LogDebug("Stream terminated");
		break;
#endif

	case PA_STREAM_READY:
		LogInfo("PulseAudio stream ready");
		pa_stream_set_read_callback(stream, onStreamRead, userdata);
		state.store(State::Connected);
		break;

	case PA_STREAM_FAILED:
		LogWarning("Connection to stream failed: " + string(pa_strerror(pa_context_errno(context))));
		scheduleReconnect();
		break;

	default:
		break;
	}
}

void PulseAudio::onStreamRead(pa_stream* stream, size_t length, void* userdata) {

	std::lock_guard<std::mutex> lock(mutex);
	rawData.clear();
	if (not length) return;

	// Read peaks.
	const void* data;
	if (pa_stream_peek(stream, &data, &length) < 0) {
		LogWarning("Stream peek failed: " + string(pa_strerror(pa_context_errno(context))));
		return;
	}

	// Hole detected.
	if (not data and length) {
		pa_stream_drop(stream);
		return;
	}

	// Parse data and convert to %.
	const float* buffer = static_cast<const float*>(data);
	size_t samples = length / sizeof(float);

	for (size_t i = 0; i < samples; ++i) {
		float v = fabsf(buffer[i]);
		if (v > 1.0f) v = 1.0f;
		rawData.push_back(roundf(v * 100.f));
	}
	pa_stream_drop(stream);
}

void PulseAudio::calculateElements() {
	// Skip processing if not connected.
	if (state.load() != State::Connected) return;

	std::lock_guard<std::mutex> lock(mutex);
	AudioActor::calculateElements();
}

void PulseAudio::calcPeak() {
	auto size(rawData.size());
	value.l = value.r = 0;
	for (size_t c = 0; c < size; ++c) {
		uint8_t v = rawData[c];
		// Even for left, odd for right.
		if (c % CHANNELS) {
			if (v > value.r) value.r = v;
		}
		else {
			if (v > value.l) value.l = v;
		}
	}
}
