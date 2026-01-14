/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      MockDirectionActor.hpp
 * @since     Oct 11, 2025
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

#include "animations/DirectionActor.hpp"

#pragma once

/**
 * MockDirectionActor
 * Minimal mock class for testing ActorDriven transitions.
 */
class MockDirectionActor : public LEDSpicer::Animations::DirectionActor {

public:

	MockDirectionActor(
		StringUMap&                parameters,
		LEDSpicer::Devices::Group* const group,
		const vector<string>&      requiredParameters
	) : DirectionActor(parameters, group, requiredParameters) {}

	virtual ~MockDirectionActor() = default;

	/// Track restart calls for testing.
	bool wasRestarted() const {
		return restarted;
	}

	/// Track draw calls for testing.
	int getDrawCount() const {
		return drawCount;
	}

	/// Reset mock state for reuse tests.
	void resetMockState() {
		restarted = false;
		drawCount = 0;
	}

	void restart() override {
		DirectionActor::restart();
		restarted = true;
	}

	void draw() override {
		DirectionActor::draw();
		drawCount++;
	}

	/**
	 * Sets stepping for testing.
	 * @param stepping
	 */
	void setStepping(FrameActor::Stepping stepping) {
		this->stepping = stepping;
	}

protected:

	void calculateElements() override {
		affectAllElements(true);
	}

private:

	bool restarted = false;
	int  drawCount = 0;
};
