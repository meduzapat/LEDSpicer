/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      mameparser.hpp
 * @since     Dec 1, 2018
 * @author    Patricio A. Rossi (MeduZa)
 * @copyright Copyright © 2018 - 2020 Patricio A. Rossi (MeduZa)
 */

#ifndef MAMEPARSER_HPP_
#define MAMEPARSER_HPP_ 1

namespace LEDSpicer {


struct Player {

	enum class Controllers : uint8_t {
		None,
		Joy,
		Doublejoy,
		Trackball,
		LightGun,
		Dial,
		Mouse,
		Keypad,
		Pedal,
		Other
	};

	/// The controller type.
	Controllers controller = Controllers::None;

	/// Number of buttons for this player.
	uint8_t buttons = 0;

	/**
	 * Convert controller code into string.
	 * @param controller
	 * @return
	 */
	string controller2str(Controllers controller) {
		switch (controller) {
		case Controllers::Joy:
			return "Joy";
		case Controllers::Doublejoy:
			return "Doublejoy";
		case Controllers::Trackball:
			return "Trackball";
		case Controllers::LightGun:
			return "LightGun";
		case Controllers::Dial:
			return "Dial";
		case Controllers::Mouse:
			return "Mouse";
		case Controllers::Keypad:
			return "Keypad";
		case Controllers::Pedal:
			return "Pedal";
		case Controllers::Other:
			return "Other";
		}
		return "None";
	}

	/**
	 * Convert a string into controller code.
	 * @param controller
	 * @return
	 */
	Controllers str2Controller(const string& controller) {
		if (controller == "Joy")
			return Controllers::Joy;
		if (controller == "Doublejoy")
			return Controllers::Doublejoy;
		if (controller == "Trackball")
			return Controllers::Trackball;
		if (controller == "LightGun")
			return Controllers::LightGun;
		if (controller == "Dial")
			return Controllers::Dial;
		if (controller == "Mouse")
			return Controllers::Mouse;
		if (controller == "Keypad")
			return Controllers::Keypad;
		if (controller == "Pedal")
			return Controllers::Pedal;
		if (controller == "Other")
			return Controllers::Other;
		throw Error("Invalid controller " + controller);
	}

	/**
	 * Covert the inner data into profile information.
	 * @return
	 */
	string toString() {
		return (controller2str(controller) + (buttons ?  "_" + to_string(buttons) : "") +  "_");
	}

};

struct Emulator {
/*
TODO: maybe one day...
	enum class Rotations : uint8_t {Degree90, Degree180, Degree270, Degree360};

	enum class Genres : uint8_t {
		Action,
		Adventure,
		Sport,
		Driving,
		Educational,
		Fighting,
		Fitness,
		Simulator,
		Gambling,
		Music,
		Pinball,
		Platformer,
		Puzzle,
		Role,
		Shooter,
		Strategy,
		Trivia,
		Other
	};
	Rotations rotate = Rotations::Degree90;
	Genres genre = Genres::Other;
*/
	vector<Player> players;
	uint8_t coins = 0;

	string toString() {
		string txt;
		for(auto&p : players)
			txt += p.toString();
		return (txt + "_" + to_string(coins));
	}
};


/**
 * LEDSpicer::mameparser
 * Handles Mame emulator.
 */
class mameparser {

public:

	mameparser();

	virtual ~mameparser();

};

} /* namespace LEDSpicer */

#endif /* MAMEPARSER_HPP_ */
