/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      FileReader.hpp
 * @since     Nov 14, 2019
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

#include "StepActor.hpp"

#ifndef FILEREADER_HPP_
#define FILEREADER_HPP_ 1

#define REQUIRED_PARAM_ACTOR_FILEREADER {"speed", "direction", "filename", "format"}

namespace LEDSpicer::Animations {

/**
 * LEDSpicer::FileReader
 */
class FileReader : public StepActor {

public:

	enum class Formats : uint8_t {rgba};

	FileReader(umap<string, string>& parameters, Group* const layout);

	virtual ~FileReader() {}

	void drawConfig();

	string Format2str(Formats format);

	Formats str2Format(const string& format);

protected:

	void calculateElements();

private:

	/// Iterator to the pair
	umap<string, vector<vector<Color>>>::iterator frames;

	/// All files on memory data by filename.
	static umap<string, vector<vector<Color>>> fileData;

	/**
	 * Loads into memory a RGBA file, an animation file that stores data in RGB values divided into frames.
	 * @param filename
	 */
	void processRGBA(const string& filename);

};

} /* namespace */

actorFactory(LEDSpicer::Animations::FileReader)

#endif /* FILEREADER_HPP_ */
