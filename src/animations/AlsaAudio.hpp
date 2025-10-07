/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      AlsaAudio.hpp
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

#ifndef ALSA_PCM_NEW_HW_PARAMS_API
	#define ALSA_PCM_NEW_HW_PARAMS_API
#endif

#include <alsa/asoundlib.h>
#include "AudioActor.hpp"

#pragma once

constexpr uint16_t SAMPLE_RATE = 48000;
constexpr float MAX_AMP        = 32767.0f;

namespace LEDSpicer::Animations {

using LEDSpicer::Utilities::Log;
using LEDSpicer::Utilities::Error;

/**
 * LEDSpicer::Animations::AlsaAudio
 */
class AlsaAudio: public AudioActor {

public:

	AlsaAudio(StringUMap& parameters, Group* const group);

	virtual ~AlsaAudio();

	void drawConfig() const override;

protected:

	/// Number of active instances sharing the PCM handle.
	static uint8_t instances;

	/// Shared PCM handle for audio capture.
	static snd_pcm_t* pcm;

	void calcPeak() override;
};

} // namespace

