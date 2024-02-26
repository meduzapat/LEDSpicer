/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      AlsaAudio.cpp
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

#include "AlsaAudio.hpp"

using namespace LEDSpicer::Animations;

snd_pcm_t* AlsaAudio::pcm    = nullptr;
uint8_t AlsaAudio::instances = 0;

AlsaAudio::AlsaAudio(umap<string, string>& parameters, Group* const group) :
	AudioActor(parameters, group) {

	++instances;

	LogInfo("Connecting to ALSA " SND_LIB_VERSION_STR);

	if (pcm) {
		LogInfo("Reusing ALSA connection");
		return;
	}

	string PCM = parameters.count("pcm") ? parameters["pcm"] : "default";

	int err;
	if ((err = snd_pcm_open(&pcm, PCM.c_str(), SND_PCM_STREAM_CAPTURE, 0)) < 0)
		throw Error("Unable to open " + PCM + ": " + string(snd_strerror(err)));

	if ((err = snd_pcm_set_params(
		pcm,
		SND_PCM_FORMAT_S16_LE,
		SND_PCM_ACCESS_RW_INTERLEAVED,
		2,
		96000,
		1,
		0
	)) < 0)
		throw Error("Unable to set parameters for " + PCM + ": " + string(snd_strerror(err)));
}

AlsaAudio::~AlsaAudio() {
	--instances;
	if (instances > 0)
		return;

	LogInfo("Closing ALSA");
	snd_pcm_close (pcm);
}

void AlsaAudio::calcPeak() {
	int16_t buffer[128];
	if (snd_pcm_readi(pcm, buffer, PEAKS) != PEAKS)
		return;

	for (uint8_t c = 0; c < 64; ++c) {
		uint8_t v = (buffer[c] / INT16_MAX) * UINT8_MAX;
		// Even for left, odd for right.
		if (c % 2) {
			if (v > value.r)
				value.r = v;
		}
		else {
			if (v > value.l)
				value.l = v;
		}
	}
}

void AlsaAudio::calcPeaks() {

	int16_t buffer[128];
	if (snd_pcm_readi(pcm, buffer, PEAKS) != PEAKS)
		return;

	uint8_t
		v = 0,
		c = 0;
	for (size_t c = 0; c < PEAKS; ++c) {
		Values value;
		// Even for left, odd for right.
		v = (buffer[c] / INT16_MAX) * UINT8_MAX;
		if (v > value.r)
			value.r = v;
		++c;
		v = (buffer[c] / INT16_MAX) * UINT8_MAX;
		if (v > value.l)
			value.l = v;
		++c;
		values.push_back(value);
	}
}

void AlsaAudio::drawConfig() {
	cout << "Type: AlsaAudio" << endl;
	AudioActor::drawConfig();
}
