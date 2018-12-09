/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      MainBase.hpp
 * @since     Nov 18, 2018
 * @author    Patricio A. Rossi (MeduZa)
 * @copyright Copyright © 2018 Patricio A. Rossi (MeduZa)
 */

// To handle daemonization.
#include <unistd.h>

// To handle c signals.
#include <csignal>
using std::signal;

#include "Messages.hpp"
#include "DataLoader.hpp"

#ifndef MAINBASE_HPP_
#define MAINBASE_HPP_ 1

namespace LEDSpicer {

/**
 * LEDSpicer::MainBase
 * Class to keep all the main extra functionality and data.
 */
class MainBase {

public:

	enum class Modes : uint8_t {
		/// Do not detach from head.
		Foreground,
		/// Dump Config and exit.
		Dump,
		/// Run as a daemon.
		Normal,
		/// Run LEDs test.
		TestLed,
		/// Run Elements test.
		TestElement
	};

	MainBase(Modes mode);

	virtual ~MainBase();

	/**
	 * Starts the the LEDs test.
	 */
	void testLeds();

	/**
	 * Starts the Elements test.
	 */
	void testElements();

	/**
	 * Starts the dump configuration.
	 */
	void dumpConfiguration();

protected:

	/// Flag for the main loop.
	static bool running;

	/// Keeps messages incoming.
	Messages messages;

	static Profile* currentProfile;

	/**
	 * Stack of profiles.
	 */
	static vector<Profile*> profiles;

	/// Keeps a list of always on elements.
	umap<string, Profile::ElementItem> alwaysOnElements;

	/// Keeps a list of always on groups.
	umap<string, Profile::GroupItem> alwaysOnGroups;

	/**
	 * Functionality for test programs.
	 * @return
	 */
	Device* selectDevice();

	/**
	 * Attempts to load profiles from a list of names.
	 * @param data
	 * @return nullptr if all failed.
	 */
	Profile* tryProfiles(const vector<string>& data);

};

} /* namespace LEDSpicer */

#endif /* MAINBASE_HPP_ */
