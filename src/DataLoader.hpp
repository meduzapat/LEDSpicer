/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file		DataLoader.hpp
 * @since		Jun 22, 2018
 * @author		Patricio A. Rossi (MeduZa)
 * @copyright	Copyright © 2018 Patricio A. Rossi (MeduZa)
 */

#ifndef DATALOADER_HPP_
#define DATALOADER_HPP_ 1

#include "config.h"
#include "utility/XMLHelper.hpp"
#include "devices/UltimarcUltimate.hpp"
#include "utility/Color.hpp"
#include "devices/Profile.hpp"
#include "animations/Serpentine.hpp"
#include "animations/Pulse.hpp"
#include "animations/Gradient.hpp"
#include "animations/Random.hpp"

#define REQUIRED_PARAM_ROOT           {"colors", "fps"}
#define REQUIRED_PARAM_COLOR          {"name", "color"}
#define REQUIRED_PARAM_DEVICE         {"name", "boardId"}
#define REQUIRED_PARAM_DEVICE_ELEMENT {"name", "type"}
#define REQUIRED_PARAM_RGB_LED        {"red", "green", "blue"}
#define REQUIRED_PARAM_LAYOUT         {"defaultProfile"}
#define REQUIRED_PARAM_PROFILE        {"defaultColorOn", "defaultColorOff", "startAnimation", "stopAnimation"}
#define REQUIRED_PARAM_NAME_ONLY      {"name"}

namespace LEDSpicer {

using Animations::Actor;
using Animations::Serpentine;
using Animations::Pulse;
using Animations::Gradient;
using Animations::Random;
using Devices::UltimarcUltimate;
using Devices::Device;
using Devices::Profile;
using Devices::Group;
using Devices::Element;

/**
 * LEDSpicer::DataLoader
 *
 * Data loader will load any data file and configuration and process it
 */
class DataLoader: public XMLHelper {

public:

	using XMLHelper::XMLHelper;

	virtual ~DataLoader() {}

	/**
	 * Reads and process the configuration file.
	 */
	void read();

	/**
	 * Returns the devices list.
	 * @return
	 */
	vector<Device*>& getDevices();

	/**
	 * Returns the layout.
	 * @return
	 */
	umap<string, Group>& getLayout();

	/**
	 * Returns the list of all registered elements.
	 * @return
	 */
	umap<string, Element*>& getElementList();

	/**
	 * Reads an profile file.
	 * @param name
	 */
	Profile* processProfile(const string& name);

	/**
	 * Returns the name of the default profile.
	 * @return
	 */
	string& getDefaultProfile();

protected:

	/// Stores devices.
	vector<Device*> devices;

	/// list with all elements by name.
	umap<string, Element*> allElements;

	/// Stores groups by name.
	umap<string, Group> layout;

	/// Stores the default profile name.
	string defaultProfile;

	/**
	 * Loads the color File
	 * @param file
	 */
	void processColorFile(const string& file);

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
	void processGroupElements(tinyxml2::XMLElement* groupNode, const string& name);

	/**
	 * Reads an animation file.
	 * @param file
	 */
	vector<Actor*> processAnimation(const string& file);

	/**
	 * Creates a device object.
	 * TODO: In a future will be cool to move devices into plugins so they are loaded dynamically.
	 * @param name
	 * @param boardId
	 * @return
	 */
	static Device* createDevice(const string& name, const string& boardId);

	/**
	 * Creates a new animation object.
	 * TODO: In a future will be cool to move animations into plugins so they are loaded dynamically.
	 * @param name
	 * @param actorData
	 * @return
	 */
	Actor* createAnimation(const string& name, umap<string, string>& actorData);

};

} /* namespace LEDSpicer */

#endif /* DATALOADER_HPP_ */
