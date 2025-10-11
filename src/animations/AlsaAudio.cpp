/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      AlsaAudio.cpp
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

#include "AlsaAudio.hpp"

using namespace LEDSpicer::Animations;

using LEDSpicer::Devices::Group;

snd_pcm_t* AlsaAudio::pcm    = nullptr;
uint8_t AlsaAudio::instances = 0;

AlsaAudio::AlsaAudio(StringUMap& parameters, Group* const group) :
	AudioActor(parameters, group) {

	++instances;

	LogInfo("Connecting to ALSA " SND_LIB_VERSION_STR);

	if (pcm) {
		LogInfo("Reusing ALSA connection");
		return;
	}

	string PCM = parameters.exists("pcm") ? parameters["pcm"] : "default";

	int err;
	// Open with non-blocking flag
	if ((err = snd_pcm_open(&pcm, PCM.c_str(), SND_PCM_STREAM_CAPTURE, SND_PCM_NONBLOCK)) < 0)
		throw Error("Unable to open ") << PCM << ": " << snd_strerror(err);

	// Calculate latency based on FPS
	// Frame time = 1000ms / FPS
	uint32_t frameTimeMs = 1000 / getFPS();
	// Buffer should be 2-3x frame time to prevent underruns
	uint32_t bufferLatencyUs = frameTimeMs * 2500;

	// Set basic parameters with appropriate latency
	if ((err = snd_pcm_set_params(
		pcm,
		SND_PCM_FORMAT_S16_LE,
		SND_PCM_ACCESS_RW_INTERLEAVED,
		CHANNELS,
		SAMPLE_RATE,
		0,  // No resampling
		bufferLatencyUs
	)) < 0)
		throw Error("Unable to set parameters for " + PCM + ": " + string(snd_strerror(err)));
	// Explicitly prepare the PCM
	if ((err = snd_pcm_prepare(pcm)) < 0)
		throw Error("Unable to prepare PCM: " + string(snd_strerror(err)));

	// Start capture
	if ((err = snd_pcm_start(pcm)) < 0)
		throw Error("Unable to start PCM: " + string(snd_strerror(err)));

	LogInfo("ALSA connected");
}

AlsaAudio::~AlsaAudio() {
	--instances;
	if (instances > 0) return;

	LogInfo("Closing ALSA");
	snd_pcm_drop(pcm);
	snd_pcm_close(pcm);
	pcm = nullptr;
}

void AlsaAudio::calcPeak() {
	// Calculate how many samples we need based on FPS
	// This gives us "fresh" audio data matching our update rate
	static const uint16_t samplesNeeded = (SAMPLE_RATE / getFPS()); // e.g., 48000/30 = 1600 samples

	int16_t buffer[samplesNeeded * CHANNELS]; // *2 for stereo
	snd_pcm_sframes_t frames = snd_pcm_readi(pcm, buffer, samplesNeeded);

	// No new data
	if (frames == -EAGAIN || frames == 0) return;

	// Handle underruns quickly and silently (common in audio apps)
	if (frames == -EPIPE) {
#ifdef DEVELOP
		LogDebug("ALSA underrun, recovering");
#endif
		// Silent recovery
		snd_pcm_recover(pcm, frames, 1);
		return;
	}

	// Bail on other errors
	if (frames < 0) {
		LogInfo("ALSA error: " + string(snd_strerror(frames)));
		return;
	}

	// Reset peak values for this frame
	value.l = value.r = 0;

	// Process actual samples received (frames * 2 for stereo)
	uint16_t samples = frames * 2;
	for (uint16_t c = 0; c < samples; ++c) {
		float v = static_cast<float>(std::abs(buffer[c])) / MAX_AMP * 100.0f;
		// Even indices = left, odd = right
		if (c % CHANNELS) {
			if (v > value.r) value.r = v;
		}
		else {
			if (v > value.l) value.l = v;
		}
	}
}

void AlsaAudio::drawConfig() const {
	cout << "Type: AlsaAudio" << endl;
	AudioActor::drawConfig();
}
