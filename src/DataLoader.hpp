/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DataLoader.hpp
 * @since     Jun 22, 2018
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2018 - 2019 Patricio A. Rossi (MeduZa)
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

#include <algorithm>

#include "config.h"
#include "utility/XMLHelper.hpp"
#include "utility/Color.hpp"
#include "devices/Profile.hpp"
#include "animations/ActorHandler.hpp"
#include "devices/DeviceHandler.hpp"
#include "inputs/InputHandler.hpp"

#ifndef DATALOADER_HPP_
#define DATALOADER_HPP_ 1

#define CONFIG_FILE PACKAGE_CONF_DIR PACKAGE ".conf"
#define ACTOR_DIR   "animations/"
#define PROFILE_DIR "profiles/"
#define INPUT_DIR   "inputs/"
#define MAXIMUM_FPS 30

#define REQUIRED_PARAM_ROOT           {"colors", "fps", "port"}
#define REQUIRED_PARAM_COLOR          {"name", "color"}
#define REQUIRED_PARAM_DEVICE         {"name", "boardId"}
#define REQUIRED_PARAM_DEVICE_ELEMENT {"name", "type"}
#define REQUIRED_PARAM_RGB_LED        {"red", "green", "blue"}
#define REQUIRED_PARAM_LAYOUT         {"defaultProfile"}
#define REQUIRED_PARAM_NAME_ONLY      {"name"}
#define REQUIRED_PARAM_MAP            {"type", "target", "trigger", "color", "filter"}

namespace LEDSpicer {

using Animations::Actor;
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
	 * Reads an profile file.
	 * @param name
	 * @param isDefault
	 */
	static Profile* processProfile(const string& name);

/* Storage */

	/// Stores devices.
	static vector<Device*> devices;

	/// list with all elements by name.
	static umap<string, Element*> allElements;

	/// Stores groups by name.
	static umap<string, Group> layout;

	/// Stores the default profile name.
	static Profile* defaultProfile;

	/// Keeps references to profiles.
	static umap<string, Profile*> profiles;

	/// Port number to use for listening.
	static string portNumber;

	/// Keeps references to device handlers.
	static umap<string, DeviceHandler*> deviceHandlers;

	/// Maps handlers with devices.
	static umap<Device*, DeviceHandler*> deviceMap;

	/// Keeps references to actor handlers.
	static umap<string, ActorHandler*> actorHandlers;

	/// Maps handlers with actors.
	static umap<Actor*, ActorHandler*> actorMap;

	/// Keeps references to inputs handlers.
	static umap<string, InputHandler*> inputHandlers;

	/// Maps handlers with actors.
	static umap<Input*, InputHandler*> inputMap;

	/// Keeps a list of global elements that can be turn on/off by input plugins.
	static umap<string, Element::Item*> controlledElements;

	/// Keeps a list of global groups that can be turn on/off by input plugins.
	static umap<string, Group::Item*> controlledGroups;

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
	 * Reads an animation file.
	 * @param file
	 */
	static vector<Actor*> processAnimation(const string& file);

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
	 * Reads an input file.
	 * @param profile
	 * @param file
	 */
	static void processInput(Profile* profile, const string& file);

	/**
	 * Loads an input object.
	 */
	static Input* createInput(umap<string, string>& inputData);

	/**
	 * Extracts input map data.
	 * @param inputNode
	 * @param input
	 */
	static void processInputMap(tinyxml2::XMLElement* inputNode, Input* input);

	/**
	 * Prepares the filenames.
	 * @param name
	 * @return
	 */
	static string createFilename(const string& name);
};

} /* namespace LEDSpicer */

#endif /* DATALOADER_HPP_ */
