/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Emitter.hpp
 * @since     Jul 8, 2018
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2018 - 2020 Patricio A. Rossi (MeduZa)
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

#include <iostream>
using std::cout;
using std::endl;

#include <cstdlib>
#include <cstring>

#include <memory>

#include "Messages.hpp"
#include "utility/XMLHelper.hpp"

#ifndef EMITTER_HPP_
#define EMITTER_HPP_ 1

#define CONFIG_FILE PACKAGE_CONF_DIR "/" PACKAGE ".conf"
#define CONTROLLERS_FILE PACKAGE_DATA_DIR "gameData.xml"

#define CONTROL "C"
#define PLAYERS "ps"
#define PLAYER  "p"
#define TYPE    "t"
#define BUTTONS "b"
#define WAYS    "w"
#define COINS   "cs"

struct PlayerData {

	string
		player  = "",
		type    = "",
		buttons = "";

	vector<string> ways;

	/**
	 * @return The dataset as a string.
	 */
	string toString();

	/**
	 * Rotates the restrictors if any.
	 */
	string rotate();
};

struct GameRecord {

	string
		players = "0",
		coins   = "0";

	vector<PlayerData> playersData;

	/**
	 * @return The dataset as a string.
	 */
	vector<string> toString();

	/**
	 * Call Rotate on every player.
	 */
	void rotate();
};

/**
 * Main function.
 * Handles command line and executes the program.
 *
 * @param argc
 * @param argv
 * @return exit code.
 */
int main(int argc, char **argv);

/**
 * Process the mame game data.
 * examples:
 *   <M n="abcheck" ps="3" cs="1"><C t="only_buttons" p="1" b="2"/><C t="only_buttons" p="2" b="2"/><C t="only_buttons" p="3" b="2"/></M>
 *   <control type="joy" player="2" buttons="1" ways="2|4|8"/>
 *   <control type="stick" player="1" buttons="4" minimum="0" maximum="255" sensitivity="30" keydelta="30" reverse="yes"/>
 *   <control type="keypad" player="1" buttons="24"/>
 *   <control type="paddle" buttons="2" minimum="0" maximum="63" sensitivity="100" keydelta="1"/>
 *   <control type="pedal" buttons="3" minimum="0" maximum="31" sensitivity="100" keydelta="1"/>
 *   <control type="dial" buttons="4" minimum="0" maximum="255" sensitivity="25" keydelta="20"/>
 *   <control type="only_buttons" buttons="7"/>
 *   <control type="mouse" player="1" minimum="0" maximum="255" sensitivity="100" keydelta="5"/>
 *   <control type="lightgun" player="1" buttons="1" minimum="0" maximum="255" sensitivity="70" keydelta="10"/>
 *   <control type="trackball" player="1" buttons="1" minimum="0" maximum="255" sensitivity="100" keydelta="10" reverse="yes"/>
 *   <control type="doublejoy" buttons="1" ways="8" ways2="8"/>
 * @param rom
 * @return
 */
GameRecord parseMame(const string& rom);

/**
 * The Controller node have information about the player's controllers
 *
	Types:
		joy
		doublejoy  << 2x joy
		triplejoy  << 3x joy
		stick      << analog
		paddle
		pedal
		lightgun
		positional
		dial
		trackball
		mouse
		all next are ignored:
		keypad
		keyboard
		mahjong
		hanafuda
		gambling
 * @param tempAttr
 * @param variant
 * @return
 */
PlayerData processControllerNode(umap<string, string>& tempAttr, string variant = "");

string mameController2ledspicer(const string& controller);

#endif /* EMITTER_HPP_ */
