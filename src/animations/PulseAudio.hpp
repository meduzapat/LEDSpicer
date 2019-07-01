/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      PulseAudio.hpp
 * @since     Dec 13, 2018
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2018 - 2019 Patricio A. Rossi (MeduZa)
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
#include <cmath>
#include <mutex>

#include "Actor.hpp"

#ifndef PULSEAUDIO_HPP_
#define PULSEAUDIO_HPP_ 1

#define REQUIRED_PARAM_ACTOR_PULSEAUDIO {"direction", "mode", "off", "low", "mid", "high", "channel"}
#define VU_MIN_ELEMETS 6
#define STREAM_NAME "Peek Reader"
#define MIN_MAX_PEAK 10
#define SAMPLE_FORMAT PA_SAMPLE_S16LE
#define RATE 11025

#define MID_POINT 60
#define LOW_POINT 30

namespace LEDSpicer {
namespace Animations {

/**
 * LEDSpicer::Animations::PulseAudio
 * Pulseaudio output plugin.
 */
class PulseAudio: public Actor {

public:

	enum class Modes : uint8_t {VuMeter, Levels, Single};

	enum Channels : uint8_t {Left = 1, Right, Both, Mono};

	PulseAudio(umap<string, string>& parameters, Group* const layout);

	virtual ~PulseAudio();

	void drawConfig();

	static Modes str2mode(const string& mode);

	static string mode2str(Modes mode);

	static Channels str2channel(const string& channel);

	static string channel2str(Channels channel);

protected:

	struct Values {
		uint16_t
			l = 0,
			r = 0;
	};

	struct UserPref {
		Color
			/// Color to use when off
			off,
			/// Color to use when low
			c00,
			/// Color to use when mid
			c50,
			/// Color to use when full
			c75;
		Modes mode;
		Channels channel;
	} userPref;


	static pa_threaded_mainloop* tml;
	static pa_context* context;
	static pa_stream* stream;

	static uint8_t
		instances,
		totalChannels;

	static string source;

	static uint16_t peak;

	/// To avoid updating when is reading the buffer.
	static std::mutex mutex;

	uint8_t total = 0;

	/**
	 * reusable buffers.
	 */
	static Values singleData;
	static vector<Values> vuData;

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

	virtual const vector<bool> calculateElements();

private:

	static void processRawData(vector<int16_t>& rawData, UserPref* userPref);

	/**
	 * Calculates VU meters.
	 */
	void vuMeters();

	/**
	 * Calculates multiple VU meters.
	 */
	void levels();

	/**
	 * Calculates a single VU for all.
	 */
	void single();

	/**
	 * Detects the color based on the percent.
	 * @param percent
	 * @return
	 */
	Color detectColor(uint8_t percent);
};

} /* namespace Animations */
} /* namespace LEDSpicer */

actorFactory(LEDSpicer::Animations::PulseAudio)

#endif /* PULSEAUDIO_HPP_ */

