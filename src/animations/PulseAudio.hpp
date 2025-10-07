/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      PulseAudio.hpp
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

#include <pulse/pulseaudio.h>
#include <pulse/error.h>
#include <mutex>

#include "AudioActor.hpp"

#pragma once

#define STREAM_NAME "Peek Reader"
#define SAMPLE_FORMAT PA_SAMPLE_FLOAT32
#define RATE 60

namespace LEDSpicer::Animations {

/**
 * LEDSpicer::Animations::PulseAudio
 * Pulseaudio output plugin.
 */
class PulseAudio: public AudioActor {

public:

	PulseAudio(StringUMap& parameters, Group* const layout);

	virtual ~PulseAudio();

	void drawConfig() const override;

protected:

	static pa_threaded_mainloop* tml;
	static pa_context* context;
	static pa_stream* stream;

	static uint8_t instances;


	static string source;

	/// To avoid updating when is reading the buffer.
	static std::mutex mutex;

	/// Raw peak data.
	static vector<uint8_t> rawData;

	static void disconnect();

	/**
	 * Callback function for when the context state is set.
	 * @param context
	 * @param userdata
	 */
	static void onContextSetState(pa_context* context, void* userdata);

	/**
	 * Callback function for when the stream is state is set.
	 * @param stream
	 * @param userdata
	 */
	static void onStreamSetState(pa_stream* stream, void* userdata);

	/**
	 * Callback function for when the stream is ready to read.
	 * @param stream
	 * @param length
	 * @param userdata
	 */
	static void onStreamRead(pa_stream* stream, size_t length, void* userdata);

	/**
	 * Callback function for when the silk into is ready.
	 * @param context
	 * @param info
	 * @param eol
	 * @param userdata
	 */
	static void onSinkInfo(pa_context* context, const pa_sink_info* info, int eol, void* userdata);

	void calcPeak() override;

	void calculateElements();

};

} // namespace

