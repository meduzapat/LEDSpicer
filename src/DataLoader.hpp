/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file		DataLoader.hpp
 * @since		Jun 22, 2018
 * @author		Patricio A. Rossi (MeduZa)
 * @copyright	Copyright © 2018 Patricio A. Rossi (MeduZa)
 */

#ifndef DATALOADER_HPP_
#define DATALOADER_HPP_ 1

#define REQUIRED_PARAM_ROOT           {"usbDebugLevel", "colorProfile"}
#define REQUIRED_PARAM_COLOR          {"name", "color"}
#define REQUIRED_PARAM_DEVICE         {"name", "boardId", "fps"}
#define REQUIRED_PARAM_DEVICE_ELEMENT {"name", "type"}
#define REQUIRED_PARAM_RGB_LED        {"red", "green", "blue"}
#define REQUIRED_PARAM_LAYOUT         {"defaultState", "stateValue"}
#define REQUIRED_PARAM_NAME_ONLY      {"name"}
#define REQUIRED_PARAM_ACTOR          {"type", "group"}

#include "utility/XMLHelper.hpp"
#include "utility/Color.hpp"
#include "devices/Group.hpp"
#include "devices/UltimarcUltimate.hpp"
#include "animations/Serpentine.hpp"
#include "animations/Pulse.hpp"
#include "config.h"

namespace LEDSpicer {

using Animations::Animation;
using Animations::Serpentine;
using Animations::Pulse;
using Devices::UltimarcUltimate;
using Devices::Device;
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
	 * Moves the Devices out.
	 * @return
	 */
	vector<Device*> getDevices();

	/**
	 * moves the layout out.
	 * @return
	 */
	vector<Group> getLayout();

	/**
	 * Moves the animations out.
	 * @return
	 */
	vector<Animation*> getAnimations();

protected:

	/// Stores devices.
	vector<Device*> devices;

	/// Maps elements by name.
	umap<string, Element> elements;

	/// Stores the layout.
	vector<Group> layout;

	/// Maps groups by name.
	umap<string, Group*> groups;

	/// Temporary stores the animations.
	vector<Animation*> animations;

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
	 * @param group
	 */
	void processGroupElement(tinyxml2::XMLElement* groupNode, Group& group);

	/**
	 * Reads an animation file.
	 * @param file
	 */
	void processAnimation(const string& file);

	/**
	 * Creates a device object.
	 * TODO: In a future will be cool to move devices into plugins so they are loaded dynamically.
	 * @param name
	 * @param boardId
	 * @param fps
	 * @return
	 */
	static Device* createDevice(const string& name, const string& boardId, const string& fps);

	/**
	 * Creates a new animation object.
	 * TODO: In a future will be cool to move animations into plugins so they are loaded dynamically.
	 * @param name
	 * @param actorData
	 * @return
	 */
	static Animation* createAnimation(const string& name, umap<string, string>& actorData);

	/**
	 * Checks if pin is contained inside pins for a device name.
	 * @param pin
	 * @param pins
	 * @param name
	 */
	static void checkValidPin(uint8_t pin, uint8_t pins, const string& name);
};

} /* namespace LEDSpicer */

#endif /* DATALOADER_HPP_ */
