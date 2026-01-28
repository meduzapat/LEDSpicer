cmake_minimum_required(VERSION 3.10)

project(LEDSpicer VERSION 0.7.3 LANGUAGES CXX)

include(GNUInstallDirs)

# Project details
set(PROJECT_NAME      "${CMAKE_PROJECT_NAME}")
set(PROJECT_VERSION   "${CMAKE_PROJECT_VERSION}")
set(PROJECT_DATA_DIR  "${CMAKE_INSTALL_FULL_DATADIR}/ledspicer/")
set(PROJECT_CONF_DIR  "${CMAKE_INSTALL_SYSCONFDIR}")
set(PROJECT_SITE      "https://github.com/meduzapat/LEDSpicer")
set(PROJECT_BUGREPORT "https://github.com/meduzapat/LEDSpicer/issues")
set(COPYRIGHT         "Copyright © 2018 - 2026 Patricio A. Rossi")
set(MAINTAINER_EMAIL  "meduzapat@netscape.net")
set(DATA_VERSION      "1.1")

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/src)

# Devices plugin installation directory
set(DEVICES_DIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/ledspicer/devices" CACHE PATH "default devices plugin install directory")

# Systemd unit install directory (arch independed)
set(SYSTEMD_DIR "${CMAKE_INSTALL_PREFIX}/lib/systemd/system" CACHE PATH "systemd unit install directory")

# Pkg-config file install directory
set(PKGCONFIG_DIR "${CMAKE_INSTALL_LIBDIR}/pkgconfig" CACHE PATH "pkg-config file install directory")

# Docs installation directory
set(INSTALL_DOCDIR "${CMAKE_INSTALL_DATAROOTDIR}/doc/ledspicer" CACHE PATH "documentation and examples install directory")

# Global compile definitions (for source code)
add_compile_definitions(
	PROJECT_NAME="${PROJECT_NAME}"
	PROJECT_VERSION="${PROJECT_VERSION}"
	PROJECT_DATA_DIR="${PROJECT_DATA_DIR}"
	PROJECT_CONF_DIR="${PROJECT_CONF_DIR}"
	PROJECT_SITE="${PROJECT_SITE}"
	PROJECT_BUGREPORT="${PROJECT_BUGREPORT}"
	COPYRIGHT="${COPYRIGHT}"
	DATA_VERSION="${DATA_VERSION}"
)

# Options for conditional features
option(ENABLE_PULSEAUDIO  "Enables the pulseaudio plugin"            ON)
option(ENABLE_ALSAAUDIO   "Enables the alsaaudio plugin"             ON)
# Devices
option(ENABLE_NANOLED     "Enables the output plugin nanoled"        OFF)
option(ENABLE_PACDRIVE    "Enables the output plugin pacdrive"       OFF)
option(ENABLE_PACLED64    "Enables the output plugin pacled64"       OFF)
option(ENABLE_ULTIMATEIO  "Enables the output plugin ultimateio"     OFF)
option(ENABLE_LEDWIZ32    "Enables the output plugin ledwiz32"       OFF)
option(ENABLE_HOWLER      "Enables the output plugin howler"         OFF)
option(ENABLE_RASPBERRYPI "Enables the output plugin raspberrypi"    OFF)
option(ENABLE_ADALIGHT    "Enables the output plugin adalight"       OFF)
option(ENABLE_MISTER      "Enables compiling MiSTer only features"   OFF)
# Development flags
option(ENABLE_DEVELOP     "Enables development mode"                 OFF)
option(ENABLE_TESTS       "Enables building and running tests"       OFF)
option(ENABLE_DRY_RUN     "Enables dry run mode using mock hardware" OFF)
option(ENABLE_BENCHMARK   "Enables displaying timing information"    OFF)

if(ENABLE_DEVELOP)
	# adds extra debugging logs
	add_compile_definitions(DEVELOP=1)
endif()

if(ENABLE_BENCHMARK)
	# adds extra debugging logs
	add_compile_definitions(BENCHMARK=1)
endif()

# Find required packages (pkgConfig Threads and dl)
find_package(PkgConfig REQUIRED)
find_package(Threads REQUIRED)

pkg_check_modules(TINYXML2     REQUIRED tinyxml2>=6.0)
if(NOT ENABLE_DRY_RUN)
	pkg_check_modules(LIBUSB   REQUIRED libusb-1.0>=1.0.22)
endif()

# Conditional packages
if(ENABLE_PULSEAUDIO)
	pkg_check_modules(LIBPULSE REQUIRED libpulse>=0.9)
endif()

if(ENABLE_ALSAAUDIO)
	pkg_check_modules(LIBALSA REQUIRED alsa>=0.2)
endif()

if(ENABLE_RASPBERRYPI)
	find_library(PIGPIO pigpio REQUIRED)
endif()

if(ENABLE_MISTER)
	add_compile_definitions(MiSTer=1)
	# MiSTer uses ALSA only (not sure at this moment 2025)
	set(ENABLE_PULSEAUDIO OFF)
endif()

# Check for tests.
set(RUN_TESTS NO)
if(ENABLE_TESTS)
	pkg_check_modules(GTEST gtest>=1.8)
	if(GTEST_FOUND)
		set(RUN_TESTS YES)
	else()
		message(FATAL_ERROR
			"Google Test >= 1.8 not found but ENABLE_TESTS is ON. "
			"Please install Google Test or disable tests with -DENABLE_TESTS=OFF"
		)
	endif()
endif()

################
# libledspicer #
################

add_library(ledspicer SHARED
	src/utilities/Log.cpp
	src/utilities/Utility.cpp
	src/utilities/Serial.cpp
	src/utilities/USB.cpp
	src/utilities/XMLHelper.cpp
	src/utilities/Speed.cpp
	src/utilities/Direction.cpp
	src/utilities/Color.cpp
	src/utilities/Colors.cpp
	src/utilities/Colorful.cpp
	src/utilities/Socks.cpp
	src/utilities/Time.cpp
	src/utilities/Message.cpp
	src/utilities/Messages.cpp
	src/utilities/Monochromatic.cpp
	src/devices/Element.cpp
	src/devices/Group.cpp
	src/devices/Device.cpp
	src/devices/DeviceSerial.cpp
	src/devices/DeviceUSB.cpp
)

if(ENABLE_DRY_RUN)
	add_compile_definitions(DRY_RUN=1)
	target_sources(ledspicer PRIVATE ${CMAKE_SOURCE_DIR}/src/utilities/FakeLibUSB.cpp)
else()
	target_include_directories(ledspicer PUBLIC ${LIBUSB_INCLUDE_DIRS})
	target_link_libraries(ledspicer PUBLIC ${LIBUSB_LIBRARIES})
endif()


set_target_properties(ledspicer PROPERTIES VERSION 1.1.0 SOVERSION 1)
install(TARGETS ledspicer LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

# Headers.
install(
	DIRECTORY src/utilities/
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/ledspicer/utilities
	FILES_MATCHING PATTERN "*.hpp"
	PATTERN "CMakeFiles" EXCLUDE
)
install(
	FILES
		src/devices/Element.hpp
		src/devices/Group.hpp
		src/devices/Device.hpp
		src/devices/DeviceSerial.hpp
		src/devices/DeviceUSB.hpp
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/ledspicer/devices
)

# Pkgconfig file.
configure_file(data/ledspicer.pc.in ledspicer.pc @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/ledspicer.pc DESTINATION ${PKGCONFIG_DIR})

##########
# Macros #
##########

# Define a macro to create generic plugins
macro(add_plugin PLUGIN_NAME PLUGIN_SRC PLUGIN_DIR)
	add_library(${PLUGIN_NAME} MODULE ${PLUGIN_SRC})
	target_link_libraries(${PLUGIN_NAME} ledspicer)
	set_target_properties(${PLUGIN_NAME} PROPERTIES PREFIX "")
	install(TARGETS ${PLUGIN_NAME} LIBRARY DESTINATION ${PLUGIN_DIR})
endmacro()

# Define a macro to create USB device plugins
macro(add_usb_device_plugin PLUGIN_NAME PLUGIN_SRC)
	add_library(${PLUGIN_NAME} MODULE ${PLUGIN_SRC})
	if(NOT ENABLE_DRY_RUN)
		target_include_directories(${PLUGIN_NAME} PRIVATE ${LIBUSB_INCLUDE_DIRS})
		target_link_libraries(${PLUGIN_NAME} ledspicer ${LIBUSB_LIBRARIES})
	else()
		target_link_libraries(${PLUGIN_NAME} ledspicer)
	endif()
	set_target_properties(${PLUGIN_NAME} PROPERTIES PREFIX "")
	install(TARGETS ${PLUGIN_NAME} LIBRARY DESTINATION ${DEVICES_DIR})
endmacro()

###############
# Executables #
###############

# LEDSpicer Daemon
# List of sources for LEDSpicer Daemon
set(LEDSPICER_SOURCES
	src/animations/Actor.cpp
	src/animations/FrameActor.cpp
	src/animations/DirectionActor.cpp
	src/animations/StepActor.cpp
	src/animations/AudioActor.cpp
	src/animations/FileReader.cpp
	src/animations/Filler.cpp
	src/animations/Gradient.cpp
	src/animations/Pulse.cpp
	src/animations/Random.cpp
	src/animations/Serpentine.cpp
	src/inputs/Input.cpp
	src/inputs/Reader.cpp
	src/inputs/Mame.cpp
	src/inputs/Impulse.cpp
	src/inputs/Actions.cpp
	src/inputs/Blinker.cpp
	src/inputs/Credits.cpp
	src/inputs/Network.cpp
	src/devices/Profile.cpp
	src/devices/transitions/Transition.cpp
	src/devices/transitions/Progressive.cpp
	src/devices/transitions/ActorDriven.cpp
	src/devices/transitions/FadeOutIn.cpp
	src/devices/transitions/CrossFade.cpp
	src/devices/transitions/Curtain.cpp
	src/Handler.cpp
	src/devices/DeviceHandler.cpp
	src/DataLoader.cpp
	src/MainBase.cpp
	src/Main.cpp
)

set(LEDSPICER_DEFINES)
set(LEDSPICER_INCLUDE_DIRS)
set(LEDSPICER_LIBS)

# PulseAudio animation (conditional)
if(ENABLE_PULSEAUDIO)
	list(APPEND LEDSPICER_SOURCES src/animations/PulseAudio.cpp)
	list(APPEND LEDSPICER_DEFINES PRIVATE PULSEAUDIO=1)
	list(APPEND LEDSPICER_INCLUDE_DIRS ${LIBPULSE_INCLUDE_DIRS})
	list(APPEND LEDSPICER_LIBS ${LIBPULSE_LIBRARIES})
endif()

# ALSA audio animation (conditional)
if(ENABLE_ALSAAUDIO)
	list(APPEND LEDSPICER_SOURCES src/animations/AlsaAudio.cpp)
	list(APPEND LEDSPICER_DEFINES PRIVATE ALSAAUDIO=1)
	list(APPEND LEDSPICER_INCLUDE_DIRS ${LIBALSA_INCLUDE_DIRS})
	list(APPEND LEDSPICER_LIBS ${LIBALSA_LIBRARIES})
endif()

add_executable(ledspicerd ${LEDSPICER_SOURCES})
target_compile_definitions(ledspicerd ${LEDSPICER_DEFINES})

if(NOT ENABLE_DRY_RUN)
	list(APPEND LEDSPICER_INCLUDE_DIRS ${TINYXML2_INCLUDE_DIRS} ${LIBUSB_INCLUDE_DIRS})
	list(APPEND LEDSPICER_LIBS ledspicer ${TINYXML2_LIBRARIES} Threads::Threads ${CMAKE_DL_LIBS} ${LIBUSB_LIBRARIES})
else()
	list(APPEND LEDSPICER_INCLUDE_DIRS ${TINYXML2_INCLUDE_DIRS})
	list(APPEND LEDSPICER_LIBS ledspicer ${TINYXML2_LIBRARIES} Threads::Threads ${CMAKE_DL_LIBS})
endif()

target_include_directories(ledspicerd PRIVATE ${LEDSPICER_INCLUDE_DIRS})
target_link_libraries(ledspicerd ${LEDSPICER_LIBS})

target_compile_definitions(ledspicerd PRIVATE DEVICES_DIR="${DEVICES_DIR}/")
install(TARGETS ledspicerd RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

# Emitter utility
add_executable(emitter src/Emitter.cpp)
target_include_directories(emitter PRIVATE ${TINYXML2_INCLUDE_DIRS})
target_link_libraries(emitter ledspicer ${TINYXML2_LIBRARIES})
install(TARGETS emitter RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

# Rotator utility
add_executable(rotator
	src/restrictors/Restrictor.cpp
	src/restrictors/RestrictorUSB.cpp
	src/restrictors/RestrictorSerial.cpp
	src/restrictors/ServoStik.cpp
	src/restrictors/UltraStik360.cpp
	src/restrictors/GPWiz49.cpp
	src/restrictors/GPWiz40RotoX.cpp
	src/restrictors/TOS428.cpp
	src/Rotator.cpp
)
if(NOT ENABLE_DRY_RUN)
	target_include_directories(rotator PRIVATE ${TINYXML2_INCLUDE_DIRS} ${LIBUSB_INCLUDE_DIRS})
	target_link_libraries(rotator ledspicer ${TINYXML2_LIBRARIES} ${LIBUSB_LIBRARIES})
else()
	target_include_directories(rotator PRIVATE ${TINYXML2_INCLUDE_DIRS})
	target_link_libraries(rotator ledspicer ${TINYXML2_LIBRARIES})
endif()
install(TARGETS rotator RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
# Ultimarc UM files
install(FILES
	data/maps/2Way.um
	data/maps/2WayV.um
	data/maps/4Way.um
	data/maps/4WayX.um
	data/maps/8Way.um
	data/maps/Analog.um
	data/maps/Mouse.um
	DESTINATION ${CMAKE_INSTALL_DATADIR}/ledspicer/umaps
)

# Input seeker utility
add_executable(inputseeker src/InputSeeker.cpp)
install(TARGETS inputseeker RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

# Process lookup utility
add_executable(processLookup src/ProcessLookup.cpp)
target_include_directories(processLookup PRIVATE ${TINYXML2_INCLUDE_DIRS})
target_link_libraries(processLookup ledspicer ${TINYXML2_LIBRARIES})
install(TARGETS processLookup RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

################################
# Device plugins (conditional) #
################################

# NanoLED output plugin
if(ENABLE_NANOLED)
	add_usb_device_plugin(UltimarcNanoLed
		"src/devices/Ultimarc/FF00SharedCode.cpp;src/devices/Ultimarc/NanoLed.cpp"
	)
endif()

# PacDrive output plugin
if(ENABLE_PACDRIVE)
	add_usb_device_plugin(UltimarcPacDrive
		"src/devices/Ultimarc/PacDrive.cpp"
	)
endif()

# PacLed64 output plugin
if(ENABLE_PACLED64)
	add_usb_device_plugin(UltimarcPacLed64
		"src/devices/Ultimarc/FF00SharedCode.cpp;src/devices/Ultimarc/PacLed64.cpp"
	)
endif()

# Ultimate I/O output plugin
if(ENABLE_ULTIMATEIO)
	add_usb_device_plugin(UltimarcUltimate
		"src/devices/Ultimarc/Ultimate.cpp"
	)
endif()

# LedWiz32 output plugin
if(ENABLE_LEDWIZ32)
	add_usb_device_plugin(LedWiz32
		"src/devices/GroovyGameGear/LedWiz32.cpp"
	)
endif()

# Howler output plugin
if(ENABLE_HOWLER)
	add_usb_device_plugin(Howler
		"src/devices/WolfWareTech/Howler.cpp"
	)
endif()

# Raspberry Pi GPIO output plugin
if(ENABLE_RASPBERRYPI)
	add_plugin(RaspberryPi
		"src/devices/RaspberryPiGPIO/RaspberryPi.cpp"
		"${DEVICES_DIR}"
	)
endif()

# Adalight output plugin
if(ENABLE_ADALIGHT)
	add_plugin(Adalight
		"src/devices/Adalight/Adalight.cpp"
		"${DEVICES_DIR}"
	)
endif()

##############################
# Documentation and examples #
##############################

install(FILES README.md COPYING DESTINATION ${INSTALL_DOCDIR})

install(FILES
	data/21-ledspicer.rules
	data/ledspicer.conf
	DESTINATION ${INSTALL_DOCDIR}/examples
)

#########################
# Custom install target #
#########################

add_custom_target(uninstall
	COMMAND ${CMAKE_COMMAND} -E echo "Checking for install manifest..."
	COMMAND test -f ${CMAKE_BINARY_DIR}/install_manifest.txt || (echo "Error: install_manifest.txt not found. Run 'make install' first." && exit 1)
	COMMAND xargs rm -fv < ${CMAKE_BINARY_DIR}/install_manifest.txt
	COMMAND ${CMAKE_COMMAND} -E echo "Uninstall complete."
	COMMENT "Uninstalling LEDSpicer"
)

####################
# systemd services #
####################

# Base service file top part.
file(WRITE ${CMAKE_BINARY_DIR}/ledspicerd.service
	"[Unit]\n"
	"Description=LEDSpicer Daemon\n"
	"After=network.target sound.target\n"
	"StartLimitIntervalSec=60s\n"
	"StartLimitBurst=3\n\n"

	"[Service]\n"
	"Type=forking\n"
	"ExecStart=${CMAKE_INSTALL_PREFIX}/bin/ledspicerd\n"
	"Restart=always\n"
	"RestartSec=5s\n"
	"ExecStop=/bin/kill -TERM $MAINPID\n"
	"TimeoutStopSec=15s\n"
)

# Optional PulseAudio support
if(ENABLE_PULSEAUDIO)
	file(APPEND ${CMAKE_BINARY_DIR}/ledspicerd.service
		"# Optional PulseAudio socket hint (daemon reconnects if unavailable) remember to change 1000 with the used ID if different\n"
		"Environment=PULSE_SERVER=unix:/run/user/1000/pulse/native\n"
	)
endif()

# Base service file bottom part.
file(APPEND ${CMAKE_BINARY_DIR}/ledspicerd.service
	"\n"
	"[Install]\n"
	"WantedBy=multi-user.target\n"
)

# processLookup service file
file(WRITE ${CMAKE_BINARY_DIR}/processlookup.service
	"[Unit]\n"
	"Description=LEDSpicer Process Lookup Daemon\n"
	"After=network.target\n\n"

	"[Service]\n"
	"Type=forking\n"
	"ExecStart=${CMAKE_INSTALL_PREFIX}/bin/processLookup\n"
	"Restart=always\n"
	"RestartSec=5s\n"
	"ExecStop=/bin/kill -TERM $MAINPID\n"
	"TimeoutStopSec=15s\n\n"

	"[Install]\n"
	"WantedBy=multi-user.target\n"
)

install(FILES ${CMAKE_BINARY_DIR}/ledspicerd.service DESTINATION ${SYSTEMD_DIR})
install(FILES ${CMAKE_BINARY_DIR}/processlookup.service DESTINATION ${SYSTEMD_DIR})

###################
# Test and report #
###################

if(RUN_TESTS)
	add_subdirectory(tests)
endif()

message(STATUS "\n--------- build environment -----------
Program      : ${PROJECT_NAME}
Version      : ${PROJECT_VERSION}
WebSite      : ${PROJECT_SITE}
DATA Version : ${DATA_VERSION}
Build system : ${CMAKE_GENERATOR}
--------- build settings -----------
C++ Compiler : ${CMAKE_CXX_COMPILER} ${CMAKE_CXX_STANDARD}
Linker       : ${CMAKE_LINKER} ${CMAKE_EXE_LINKER_FLAGS} ${libraries}
Prefix       : ${CMAKE_INSTALL_PREFIX}
Data dir     : ${PROJECT_DATA_DIR}
Config dir   : ${PROJECT_CONF_DIR}
Run tests    : ${ENABLE_TESTS}
------ Special modes --------
Develop mode : ${ENABLE_DEVELOP}
Dry run mode : ${ENABLE_DRY_RUN}
MiSTer code  : ${ENABLE_MISTER}
---- Conditional Actors -----
PULSE AUDIO  : ${ENABLE_PULSEAUDIO}
ALSA AUDIO   : ${ENABLE_ALSAAUDIO}
--------- Devices -----------
Devices dir  : ${DEVICES_DIR}
NanoLed      : ${ENABLE_NANOLED}
PacDrive     : ${ENABLE_PACDRIVE}
PacLed64     : ${ENABLE_PACLED64}
Ultimate IO  : ${ENABLE_ULTIMATEIO}
LedWiz 32    : ${ENABLE_LEDWIZ32}
Howler       : ${ENABLE_HOWLER}
Raspberry Pi : ${ENABLE_RASPBERRYPI}
Adalight     : ${ENABLE_ADALIGHT}
")