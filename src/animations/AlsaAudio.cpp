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

snd_pcm_t* AlsaAudio::pcm     = nullptr;
uint8_t AlsaAudio::instances = 0;
string AlsaAudio::pcmName;

AlsaAudio::AlsaAudio(StringUMap& parameters, Group* const group) :
	AudioActor(parameters, group) {

	++instances;
	if (pcm) return;

	if (pcmName.empty())
		pcmName = parameters.exists("pcm") ? parameters["pcm"] : "default";

	// Does not throw: calcPeak retries if the device is not ready or later drops.
	connect();
}

AlsaAudio::~AlsaAudio() {
	if (--instances) return;
	disconnect();
}

bool AlsaAudio::connect() {

	int err;
	// Non-blocking so a missing device fails fast.
	if ((err = snd_pcm_open(&pcm, pcmName.c_str(), SND_PCM_STREAM_CAPTURE, SND_PCM_NONBLOCK)) < 0) {
		LogDebug("Unable to open " + pcmName + ": " + snd_strerror(err));
		pcm = nullptr;
		return false;
	}

	// Buffer should be 2-3x the frame time to prevent underruns.
	uint32_t bufferLatencyUs = (1000 / getFPS()) * 2500;

	if ((err = snd_pcm_set_params(
		pcm,
		SND_PCM_FORMAT_S16_LE,
		SND_PCM_ACCESS_RW_INTERLEAVED,
		CHANNELS,
		SAMPLE_RATE,
		0,  // No resampling
		bufferLatencyUs
	)) < 0) {
		LogDebug("Unable to set parameters for " + pcmName + ": " + string(snd_strerror(err)));
		disconnect();
		return false;
	}

	if ((err = snd_pcm_prepare(pcm)) < 0) {
		LogDebug("Unable to prepare PCM: " + string(snd_strerror(err)));
		disconnect();
		return false;
	}

	if ((err = snd_pcm_start(pcm)) < 0) {
		LogDebug("Unable to start PCM: " + string(snd_strerror(err)));
		disconnect();
		return false;
	}

	LogInfo("ALSA connected to " + pcmName);
	return true;
}

void AlsaAudio::disconnect() {
	if (not pcm) return;
	snd_pcm_drop(pcm);
	snd_pcm_close(pcm);
	pcm = nullptr;
}

void AlsaAudio::calcPeak() {

	// Reconnect after a device loss; skip the frame while unavailable.
	if (not pcm and not connect()) return;

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

	// Device gone: drop so the next frame reconnects.
	if (frames < 0) {
		LogWarning("ALSA error: " + string(snd_strerror(frames)) + ", reconnecting");
		disconnect();
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
	cout << "AlsaAudio" << endl;
	AudioActor::drawConfig();
}
