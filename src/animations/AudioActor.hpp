/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      AudioActor.hpp
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

#include <cmath>
#include "utility/Direction.hpp"
#include "Actor.hpp"

#ifndef AUDIOACTOR_HPP_
#define AUDIOACTOR_HPP_ 1

#define REQUIRED_PARAM_ACTOR_AUDIO {"mode", "off", "low", "mid", "high", "channel"}
#define VU_MIN_ELEMETS 6

#define HIG_POINT 95
#define MID_POINT 55
#define LOW_POINT 25

namespace LEDSpicer::Animations {

/**
 * LEDSpicer::Inputs::AudioActor
 * Audio plugins uses the direction to know in what direction the effect need to draw,
 * only forward and backward is used.
 */
class AudioActor: public Actor, public Direction {

public:

	enum class Modes : uint8_t {VuMeter, Levels, Single, Wave};

	enum Channels : uint8_t {Left = 1, Right, Both, Mono};

	AudioActor(umap<string, string>& parameters, Group* const group);

	virtual ~AudioActor() = default;

	void drawConfig() override;

	static Modes str2mode(const string& mode);

	static string mode2str(Modes mode);

	static Channels str2channel(const string& channel);

	static string channel2str(Channels channel);

	void restart() override;

	const uint16_t getFullFrames() const override;

	const float getRunTime() const override;

protected:

	struct Values {
		uint8_t
			l = 0,
			r = 0;
	};

	static uint8_t lastF;
	static uint8_t lastFs;

	/// Preprocessed value.
	static Values value;

	/// Preprocessed values.
	static vector<Values> values;

	/// Total elements per side, stereo or mono.
	uint8_t totalElements = 0;

	struct UserPref {
		const Color
			/// Color to use when off
			& off,
			/// Color to use when low
			& c00,
			/// Color to use when mid
			& c50,
			/// Color to use when full
			& c75;
		Modes mode;
		Channels channel;
	} userPref;

	/**
	 * Stores information for Wave mode.
	 */
	vector<Color> waveData;

	/**
	 * Detects the color based on the percent.
	 * @param percent
	 * @param gradient true for smooth gradient, lines with inside gradients.
	 * @return
	 */
	Color detectColor(uint8_t percent, bool gradient = true);

	void calculateElements() override;

	/**
	 * Delete the peak information.
	 */
	void resetPeak();

	/**
	 * Delete the peaks information.
	 */
	void resetPeaks();

	/**
	 * Calculates and returns the peak out of raw data.
	 * @return
	 */
	virtual void calcPeak() = 0;

	/**
	 * Calculates and returns the peaks per channel out of raw data.
	 * @return
	 */
	virtual void calcPeaks() = 0;

#ifdef DEVELOP
	/**
	 * for debug.
	 */
	void displayPeak();

	/**
	 * for debug.
	 */
	void displayPeaks();
#endif

	/**
	 * Calculates a single VU for all.
	 */
	void single();

	/**
	 * Calculates VU meters.
	 */
	void vuMeters();

	/**
	 * Calculates multiple VU meters.
	 */
	void levels();

	/**
	 * Calculates waves
	 */
	void waves();

};

} /* namespace */

#endif /* AUDIOACTOR_HPP_ */
