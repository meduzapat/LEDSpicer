/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DataLoader.hpp
 * @since     Jun 22, 2018
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

#include "utilities/Messages.hpp"
#include "utilities/XMLHelper.hpp"
#ifdef ALSAAUDIO
#include "animations/AlsaAudio.hpp"
#endif
#include "animations/FileReader.hpp"
#include "animations/Filler.hpp"
#include "animations/Gradient.hpp"
#include "animations/Pulse.hpp"
#ifdef PULSEAUDIO
#include "animations/PulseAudio.hpp"
#endif
#include "animations/Random.hpp"
#include "animations/Serpentine.hpp"
#include "devices/transitions/FadeOutIn.hpp"
#include "devices/transitions/CrossFade.hpp"
#include "devices/transitions/Curtain.hpp"
#include "devices/DeviceHandler.hpp"
#include "inputs/InputHandler.hpp"

#pragma once

#define ACTOR_DIR   "/animations/"
#define PROFILE_DIR "/profiles/"
#define INPUT_DIR   "/inputs/"
#define MAXIMUM_FPS 60

#define PARAM_FPS             "fps"
#define PARAM_COLORS          "colors"
#define PARAM_COLOR           "color"
#define PARAM_RANDOM_COLORS   "randomColors"
#define PARAM_PORT            "port"
#define PARAM_LOG_LEVEL       "logLevel"
#define PARAM_NAME            "name"
#define PARAM_DEFAULT_COLOR   "defaultColor"
#define PARAM_LED             "led"
#define PARAM_TIMED           "solenoid"
#define PARAM_TIME_ON         "timeOn"
#define PARAM_RED             "red"
#define PARAM_GREEN           "green"
#define PARAM_BLUE            "blue"
#define PARAM_STRIP           "stripSize"
#define PARAM_POSITION        "position"
#define PARAM_POSITIONS       "positions"
#define PARAM_COLORFORMAT     "colorFormat"
#define PARAM_DEFAULT_PROFILE "defaultProfile"
#define PARAM_USER_ID         "userId"
#define PARAM_GROUP_ID        "groupId"
#define PARAM_FILTER          "filter"
#define PARAM_BRIGHTNESS      "brightness"

#define NODE_DEVICES           "devices"
#define NODE_DEVICE            "device"
#define NODE_ELEMENT           "element"
#define NODE_LAYOUT            "layout"
#define NODE_GROUP             "group"
#define NODE_COLOR             "color"
#define NODE_ANIMATIONS        "animations"
#define NODE_ANIMATION         "animation"
#define NODE_INPUTS            "inputs"
#define NODE_INPUT             "input"
#define NODE_ACTOR             "actor"
#define NODE_TRANSITION        "transition"

#define REQUIRED_PARAM_ROOT           {"colors", "fps", "port", "userId", "groupId"}
#define REQUIRED_PARAM_COLOR          {"name", "color"}
#define REQUIRED_PARAM_DEVICE         {"name"}
#define REQUIRED_PARAM_DEVICE_ELEMENT {"name", "type"}
#define REQUIRED_PARAM_RGB_LED        {"red", "green", "blue"}
#define REQUIRED_PARAM_LAYOUT         {"defaultProfile"}
#define REQUIRED_PARAM_NAME_ONLY      {"name"}
#define REQUIRED_PARAM_MAP            {"type", "target", "trigger"}

#define EMPTY_PROFILE     "profile_"
#define DEFAULT_COLOR     "White"
#define DEFAULT_ENDCYCLES 1
#define DEFAULT_ENDTIME   5
#define DEFAULT_SOLENOID  50
#define DEFAULT_ORDER     "rgb"

namespace LEDSpicer {

using namespace Utilities;
using namespace Animations;
using namespace Devices;
using namespace Inputs;
using namespace Transitions;

/**
 * LEDSpicer::DataLoader
 *
 * Data loader will load any data file and configuration and process it.
 * It also works as an storage for the loaded files.
 */
class DataLoader: public XMLHelper {

public:

	enum class Modes : uint8_t {
		/// Do not detach from head.
		Foreground,
		/// Dump Config and exit.
		Dump,
		/// Dump Profile and exit.
		Profile,
		/// Run as a daemon.
		Normal,
		/// Run LEDs test.
		TestLed,
		/// Run Elements test.
		TestElement
	};

	using XMLHelper::XMLHelper;

	virtual ~DataLoader() = default;

	/**
	 * Reads and process the configuration file.
	 *
	 * @param testProfile if set will load this profile instead of the default one.
	 */
	void readConfiguration(const string& testProfile);

	/**
	 * Check if a profile exists in the cache.
	 *
	 * @param name
	 * @return the profile or nullptr
	 */
	static Profile* getProfileFromCache(const string& name);

	/**
	 * Reads a profile file form disk or cache.
	 * @param name
	 * @param extra, if set will use it to differentiate from other profiles with the same name, only used while crafting profiles.
	 * @throws Error if the profile is invalid.
	 */
	static Profile* processProfile(const string& name, const string& extra = "");

	/**
	 * @param profile
	 * @return the transition for this profile or null.
	 */
	static Transition* getTransitionFromCache(Profile* profile);

	/**
	 * Process a transition.
	 * @param profile
	 * @param settings the setting to use to create the transition obj
	 */
	static void processTransition(Profile* profile, const StringUMap& settings);

	/**
	 * Removes a transition from the cache and frees its memory.
	 * @param profile the profile that owns the transition in cache.
	 */
	static void removeTransitionFromCache(Profile* profile);

	/**
	 * Reads an animation file.
	 *
	 * @param file the file name to load
	 * @param extra used to differentiate between normal animations and modified animations like transitions.
	 * @return
	 */
	static vector<Actor*> processAnimation(const string& file, const string& extra = "");

	/**
	 * Reads an input file.
	 * @param profile
	 * @param file
	 */
	static void processInput(Profile* profile, const string& file);

	/**
	 * Returns the current mode.
	 * @return
	 */
	static Modes getMode();

	/**
	 * Sets the mode.
	 * @param mode
	 */
	static void setMode(Modes mode);

	/**
	 * Sets the interval ms.
	 * @param waitTime
	 */
	static void setInterval(uint8_t waitTime);

	/**
	 * Deletes all cached objects.
	 */
	static void destroyCache();

/* Storage */

	/// Port number to use for listening.
	static string portNumber;

	/// Keeps the milliseconds to wait.
	static milliseconds waitTime;

protected:

	/// Stores the running mode.
	static Modes mode;

	/// Keeps references to profiles.
	static ProfilePtrUMap profilesCache;

	/// Keeps references to inputs (handled in input handlers).
	static InputPtrUMap inputCache;

	/// Stores the transitions for each unique profile.
	static unordered_map<Profile*, Transition*> transitions;

	/**
	 * Loads the color File
	 * @param file
	 */
	static void processColorFile(const string& file);

	/**
	 * Reads the devices section from config.
	 */
	void processDevices();

	/**
	 * Reads the elements from device.
	 * @param deviceNode
	 * @param device
	 */
	void processDeviceElements(tinyxml2::XMLElement* deviceNode, Device* device);

	/**
	 * Reads the layout and group sections from config.
	 * @return the default profile name.
	 */
	const string processLayout();

	/**
	 * Reads elements from groups.
	 * @param groupNode
	 * @param name
	 */
	void processGroupElements(tinyxml2::XMLElement* groupNode, Group& group);

	/**
	 * Creates a device object.
	 * @param name
	 * @param boardId
	 * @return
	 */
	static Device* createDevice(StringUMap& deviceData);

	/**
	 * Creates a new animation object.
	 * @param actorData
	 * @return
	 */
	static Actor* createAnimation(StringUMap& actorData);

	/**
	 * Extract a list of input sources (listen events in the kernel)
	 * @param inputName current input file
	 * @param inputNode the input node
	 * @return an array with the sources.
	 * @throws exception if not found or is not valid.
	 */
	static vector<string> processInputSources(const string& inputName, tinyxml2::XMLElement* inputNode);

	/**
	 * Decorates input map data.
	 * @param inputNode XML node from where the maps will be extracted
	 * @param inputMap where the new maps will be stored.
	 * @param id only used to differentiate sources.
	 * @return
	 */
	static void processInputMap(tinyxml2::XMLElement* inputNode, ItemPtrUMap& inputMaps, const string& id = "");

	/**
	 * Prepares the filenames.
	 * @param name
	 * @return
	 */
	static string createFilename(const string& name);

	/**
	 * Drops root privileges if any and moves the directory to /.
	 * @param uid new user id
	 * @param gid new group id
	 */
	void static dropRootPrivileges(uid_t uid, gid_t gid);
};

} // namespace

