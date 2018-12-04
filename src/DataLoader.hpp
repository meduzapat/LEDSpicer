/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      DataLoader.hpp
 * @since     Jun 22, 2018
 * @author    Patricio A. Rossi (MeduZa)
 * @copyright Copyright © 2018 Patricio A. Rossi (MeduZa)
 */

#include <algorithm>

#include "config.h"
#include "utility/XMLHelper.hpp"
#include "utility/Color.hpp"
#include "devices/Profile.hpp"
#include "animations/Serpentine.hpp"
#include "animations/Pulse.hpp"
#include "animations/Gradient.hpp"
#include "animations/Random.hpp"
#include "animations/Filler.hpp"
#include "devices/Ultimarc/PacDrive.hpp"
#include "devices/Ultimarc/Ultimate.hpp"
#include "devices/LedWiz/LedWiz32.hpp"

#ifndef DATALOADER_HPP_
#define DATALOADER_HPP_ 1

#define CONFIG_FILE PACKAGE_CONF_DIR "/" PACKAGE ".conf"

#define REQUIRED_PARAM_ROOT           {"colors", "fps", "port"}
#define REQUIRED_PARAM_COLOR          {"name", "color"}
#define REQUIRED_PARAM_DEVICE         {"name", "boardId"}
#define REQUIRED_PARAM_DEVICE_ELEMENT {"name", "type"}
#define REQUIRED_PARAM_RGB_LED        {"red", "green", "blue"}
#define REQUIRED_PARAM_LAYOUT         {"defaultProfile"}
#define REQUIRED_PARAM_NAME_ONLY      {"name"}

namespace LEDSpicer {

using Animations::Actor;
using Animations::Serpentine;
using Animations::Pulse;
using Animations::Gradient;
using Animations::Random;
using Animations::Filler;
using Devices::Device;
using Devices::Profile;
using Devices::Group;
using Devices::Element;
using Devices::Ultimarc::Ultimate;
using Devices::Ultimarc::PacDrive;
using Devices::LedWiz::LedWiz32;

/**
 * LEDSpicer::DataLoader
 *
 * Data loader will load any data file and configuration and process it.
 * It also works as an storage for the loaded files.
 */
class DataLoader: public XMLHelper {

public:

	using XMLHelper::XMLHelper;

	virtual ~DataLoader() {}

	/**
	 * Reads and process the configuration file.
	 */
	void readConfiguration();

	/**
	 * Reads an profile file.
	 * @param name
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

protected:

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
	 * TODO: In a future will be cool to move devices into plugins so they are loaded dynamically.
	 * @param name
	 * @param boardId
	 * @return
	 */
	static Device* createDevice(umap<string, string>& deviceData);

	/**
	 * Creates a new animation object.
	 * TODO: In a future will be cool to move animations into plugins so they are loaded dynamically.
	 * @param name
	 * @param actorData
	 * @return
	 */
	static Actor* createAnimation(const string& name, umap<string, string>& actorData);

	/**
	 * Prepares the filenames.
	 * @param name
	 * @return
	 */
	static string createFilename(const string& name);
};

} /* namespace LEDSpicer */

#endif /* DATALOADER_HPP_ */
