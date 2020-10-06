/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DataLoader.hpp
 * @since     Jun 22, 2018
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

// To handle daemonization and uid/gid.
#include <unistd.h>

#include <algorithm>

#include <chrono>
using std::chrono::milliseconds;
#include <thread>

#include "config.h"
#include "utility/XMLHelper.hpp"
#include "utility/Color.hpp"
#include "devices/Profile.hpp"
#include "animations/ActorHandler.hpp"
#include "devices/DeviceHandler.hpp"
#include "inputs/InputHandler.hpp"

#ifndef DATALOADER_HPP_
#define DATALOADER_HPP_ 1

#define ACTOR_DIR   "animations/"
#define PROFILE_DIR "profiles/"
#define INPUT_DIR   "inputs/"
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
#define PARAM_RED             "red"
#define PARAM_GREEN           "green"
#define PARAM_BLUE            "blue"
#define PARAM_DEFAULT_PROFILE "defaultProfile"
#define PARAM_USER_ID         "userId"
#define PARAM_GROUP_ID        "groupId"
#define PARAM_FILTER          "filter"

#define NODE_DEVICES           "devices"
#define NODE_DEVICE            "device"
#define NODE_ELEMENT           "element"
#define NODE_LAYOUT            "layout"
#define NODE_GROUP             "group"
#define NODE_COLOR             "color"
#define NODE_ANIMATIONS        "animations"
#define NODE_ANIMATION         "animation"
#define NODE_START_TRANSITION  "startTransition"
#define NODE_END_TRANSITION    "endTransition"
#define NODE_START_TRANSITIONS "startTransitions"
#define NODE_END_TRANSITIONS   "endTransitions"
#define NODE_INPUTS            "inputs"
#define NODE_INPUT             "input"
#define NODE_ACTOR             "actor"

#define REQUIRED_PARAM_ROOT           {"colors", "fps", "port", "userId", "groupId"}
#define REQUIRED_PARAM_COLOR          {"name", "color"}
#define REQUIRED_PARAM_DEVICE         {"name", "boardId"}
#define REQUIRED_PARAM_DEVICE_ELEMENT {"name", "type"}
#define REQUIRED_PARAM_RGB_LED        {"red", "green", "blue"}
#define REQUIRED_PARAM_LAYOUT         {"defaultProfile"}
#define REQUIRED_PARAM_NAME_ONLY      {"name"}
#define REQUIRED_PARAM_MAP            {"type", "target", "trigger"}

#define EMPTY_PROFILE    "profile_"
#define DEFAULT_COLOR     "White"
#define DEFAULT_ENDCYCLES 1
#define DEFAULT_ENDTIME   5

namespace LEDSpicer {

using Animations::Actor;
using Animations::FrameActor;
using Animations::ActorHandler;
using Devices::DeviceHandler;
using Devices::Device;
using Devices::Profile;
using Devices::Group;
using Devices::Element;
using Devices::Device;
using Inputs::Input;
using Inputs::InputHandler;

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
	 */
	void readConfiguration();

	/**
	 * Reads a profile file form disk or cache.
	 * @param name
	 * @param extra, if set will use it to differentiate from other profiles with the same name.
	 */
	static Profile* processProfile(const string& name, const string& extra = "");

	/**
	 * Reads an animation file.
	 * @param file
	 */
	static vector<Actor*> processAnimation(const string& file);

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

/* Storage */

	/// list with all elements by name.
	static umap<string, Element*> allElements;

	/// Stores groups by name.
	static umap<string, Group> layout;

	/// Stores the default profile name.
	static Profile* defaultProfile;

	/// Keeps references to profiles.
	static umap<string, Profile*> profilesCache;

	/// Port number to use for listening.
	static string portNumber;

	/// Stores devices.
	static vector<Device*> devices;

	/// Keeps references to device handlers by device type.
	static umap<string, DeviceHandler*> deviceHandlers;

	/// Keeps references to actor handlers by actor name.
	static umap<string, ActorHandler*> actorHandlers;

	/// Keeps references to inputs handlers by input name.
	static umap<string, InputHandler*> inputHandlers;

	/// Keeps a list of global elements and groups that can be turn on/off by input plugins.
	static umap<string, Items*> controlledItems;

	/// Keeps the milliseconds to wait.
	static milliseconds waitTime;

protected:

	static Modes mode;

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
	 */
	void processLayout();

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
	static Device* createDevice(umap<string, string>& deviceData);

	/**
	 * Creates a new animation object.
	 * @param actorData
	 * @return
	 */
	static Actor* createAnimation(umap<string, string>& actorData);

	/**
	 * Extracts input map data.
	 * @param inputNode
	 * @param input
	 */
	static umap<string, Items*> processInputMap(tinyxml2::XMLElement* inputNode);

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

} /* namespace LEDSpicer */

#endif /* DATALOADER_HPP_ */
