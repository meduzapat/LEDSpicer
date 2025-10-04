/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Defaults.hpp
 * @since     Jul 17, 2025
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2018 - 2025 Patricio A. Rossi (MeduZa)
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

// For ints.
#include <cstdint>
// For strerror
#include <cstring>
#include <iomanip>
#include <algorithm>
#include <cmath>

// To handle c signals.
#include <csignal>
using std::signal;

// To handle unordered map.
#include <unordered_map>

// To handle sets
#include <unordered_set>
using std::unordered_set;

#include <array>
using std::array;

#include <vector>
using std::vector;

// To handle flies
#include <fstream>

// to handle special formats.
#include <sstream>

// for setenv, close, daemonization and uid/gid.
#include <unistd.h>

// To handle strings
#include <string>
using std::string;
using std::to_string;
using std::stoi;

// To handle IO stream.
#include <iostream>
using std::cout;
using std::cerr;
using std::endl;

// For time handling.
#include <chrono>
using std::chrono::milliseconds;
using std::chrono::time_point;
using std::chrono::system_clock;
using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;

#include <thread>
using std::this_thread::sleep_for;

#pragma once

template<
	class Key,
	class T,
	class Hash      = std::hash<Key>,
	class KeyEqual  = std::equal_to<Key>,
	class Allocator = std::allocator<std::pair<const Key, T>>
>
class unordered_map : public std::unordered_map<Key, T, Hash, KeyEqual, Allocator> {
public:
	using base_type = std::unordered_map<Key, T, Hash, KeyEqual, Allocator>;
	using base_type::base_type; // Inherit all constructors

	bool exists(const Key& key) const {
		return this->find(key) != this->end();
	}

	template<class K>
	bool exists(const K& key) const {
		return this->find(key) != this->end();
	}
};

#define PERCENT(x , y) ((x) * 100 / (y))

#define WAYS_INDICATOR "WAYS"

/// Separators.
#define SEPARATOR "------------------------------"
constexpr char GROUP_SEPARATOR    = 29;  // Group Separator (GS)
constexpr char FIELD_SEPARATOR    = 30;  // Record Separator (RS)
constexpr char RECORD_SEPARATOR   = 31;  // Unit Separator (US)
constexpr char ID_SEPARATOR       = ','; // Comma
constexpr char ID_GROUP_SEPARATOR = '|'; // Pipe

/**
 * Flags
 * @{
 */
/// Flag indicating the absence of animations.
#define FLAG_NO_ANIMATIONS  1
/// Flag indicating the absence of inputs.
#define FLAG_NO_INPUTS      2
/// Flag indicating the absence of transitions.
#define FLAG_NO_TRANSITIONS 4
/// Flag indicating the absence of a rotator.
#define FLAG_NO_ROTATOR     8
/// Flag indicating a force reload.
#define FLAG_FORCE_RELOAD   16
/// Flag indicating a replacement.
#define FLAG_REPLACE        32
/// @}

/**
 * Transition types.
 * @{
 */
/// No transition.
#define TRANSITION_NONE  0
/// Transition with a morph effect between two profiles.
#define TRANSITION_MORPH 1
/// Transition with a fade effect between two profiles.
#define TRANSITION_FADE  2

/// @}

/**
 * Connection settings
 * @{
 */
/// Localhost IP address.
constexpr char LOCALHOST[]   = "127.0.0.1";
/// Number of bytes to read/write in a single operation.
constexpr size_t BUFFER_SIZE = 256;
/// Ports to scan: ignoring old or unrelated like /dev/ttyS
constexpr array<const char*, 2> DEFAULT_SERIAL_PORTS{"ttyUSB", "ttyACM"};
/// Maximum number of serial ports to scan (ttyUSB1..ttyUSB5, ttyACM1..ttyACM5)
#define MAX_SERIAL_PORTS_TO_SCAN 5
/// @}

using Uint8UMap  = unordered_map<string, uint8_t>;
using StringUMap = unordered_map<string, string>;
