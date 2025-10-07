cmake_minimum_required(VERSION 3.10)
project(LEDSpicer VERSION 0.7.0 LANGUAGES CXX)

include(GNUInstallDirs)

# Metadata (for substitutions and compile definitions)
set(PROJECT_NAME      "${CMAKE_PROJECT_NAME}")
set(PROJECT_VERSION   "${CMAKE_PROJECT_VERSION}")
set(PROJECT_DATA_DIR  "${CMAKE_INSTALL_FULL_DATADIR}/ledspicer")
set(PROJECT_CONF_DIR  "${CMAKE_INSTALL_SYSCONFDIR}")
set(PROJECT_SITE      "https://github.com/meduzapat/LEDSpicer")
set(PROJECT_BUGREPORT "https://github.com/meduzapat/LEDSpicer/issues")
set(COPYRIGHT         "Copyright © 2018 - 2026 Patricio A. Rossi")
set(DATA_VERSION      "1.1")

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/src)

# Plugin installation directories
set(ACTORS_DIR  "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/ledspicer/actors")
set(DEVICES_DIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/ledspicer/devices")
set(INPUTS_DIR  "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/ledspicer/inputs")

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

find_package(PkgConfig REQUIRED)
# Find required packages
pkg_check_modules(TINYXML2     REQUIRED tinyxml2>=6.0)
pkg_check_modules(PTHREADSTUBS REQUIRED pthread-stubs)
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
	set(ENABLE_PULSEAUDIO OFF)
endif()

# Find dl library
find_library(DL dl REQUIRED)

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

# Shared library: libledspicer
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
	src/utilities/Monochromatic.cpp
	src/restrictors/Restrictor.cpp
	src/restrictors/RestrictorUSB.cpp
	src/restrictors/RestrictorSerial.cpp
	src/devices/Element.cpp
	src/devices/Group.cpp
	src/devices/Profile.cpp
	src/devices/Device.cpp
	src/devices/DeviceSerial.cpp
	src/devices/DeviceUSB.cpp
	src/animations/Actor.cpp
	src/animations/FrameActor.cpp
	src/animations/DirectionActor.cpp
	src/animations/StepActor.cpp
	src/animations/AudioActor.cpp
	src/inputs/Input.cpp
	src/inputs/Reader.cpp
	src/utilities/Message.cpp
	src/utilities/Messages.cpp
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
install(DIRECTORY src/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/ledspicer
	FILES_MATCHING PATTERN "*.hpp"
	PATTERN "CMakeFiles" EXCLUDE
)

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
add_executable(ledspicerd
	src/devices/transitions/Transition.cpp
	src/devices/transitions/ProgressiveTransition.cpp
	src/devices/transitions/ActorDriven.cpp
	src/devices/transitions/FadeOutIn.cpp
	src/devices/transitions/CrossFade.cpp
	src/devices/transitions/Curtain.cpp
	src/Handler.cpp
	src/animations/ActorHandler.cpp
	src/devices/DeviceHandler.cpp
	src/inputs/InputHandler.cpp
	src/DataLoader.cpp
	src/MainBase.cpp
	src/Main.cpp
)
if(NOT ENABLE_DRY_RUN)
	target_include_directories(ledspicerd PRIVATE ${TINYXML2_INCLUDE_DIRS} ${PTHREADSTUBS_INCLUDE_DIRS} ${LIBUSB_INCLUDE_DIRS})
	target_link_libraries(ledspicerd ledspicer ${TINYXML2_LIBRARIES} ${PTHREADSTUBS_LIBRARIES} ${DL} ${LIBUSB_LIBRARIES})
else()
	target_include_directories(ledspicerd PRIVATE ${TINYXML2_INCLUDE_DIRS} ${PTHREADSTUBS_INCLUDE_DIRS})
	target_link_libraries(ledspicerd ledspicer ${TINYXML2_LIBRARIES} ${PTHREADSTUBS_LIBRARIES} ${DL})
endif()
target_compile_definitions(ledspicerd PRIVATE
	ACTORS_DIR="${ACTORS_DIR}/"
	DEVICES_DIR="${DEVICES_DIR}/"
	INPUTS_DIR="${INPUTS_DIR}/"
)
install(TARGETS ledspicerd RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

# Emitter utility
add_executable(emitter src/Emitter.cpp)
target_include_directories(emitter PRIVATE ${TINYXML2_INCLUDE_DIRS})
target_link_libraries(emitter ledspicer ${TINYXML2_LIBRARIES})
install(TARGETS emitter RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

# Rotator utility
add_executable(rotator
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

#################
# Actor plugins #
#################

# Gradient animation plugin
add_plugin(Gradient
	"src/animations/Gradient.cpp"
	"${ACTORS_DIR}"
)

# Pulse animation plugin
add_plugin(Pulse
	"src/animations/Pulse.cpp"
	"${ACTORS_DIR}"
)

# Serpentine animation plugin
add_plugin(Serpentine
	"src/animations/Serpentine.cpp"
	"${ACTORS_DIR}"
)

# Random animation plugin
add_plugin(Random
	"src/animations/Random.cpp"
	"${ACTORS_DIR}"
)

# Filler animation plugin
add_plugin(Filler
	"src/animations/Filler.cpp"
	"${ACTORS_DIR}"
)

# FileReader animation plugin
add_plugin(FileReader
	"src/animations/FileReader.cpp"
	"${ACTORS_DIR}"
)

# PulseAudio animation plugin (conditional)
if(ENABLE_PULSEAUDIO)
	add_library(PulseAudio MODULE src/animations/PulseAudio.cpp)
	target_include_directories(PulseAudio PRIVATE ${LIBPULSE_INCLUDE_DIRS})
	target_link_libraries(PulseAudio ledspicer ${LIBPULSE_LIBRARIES})
	set_target_properties(PulseAudio PROPERTIES PREFIX "")
	install(TARGETS PulseAudio LIBRARY DESTINATION ${ACTORS_DIR})
endif()

# ALSA audio animation plugin (conditional)
if(ENABLE_ALSAAUDIO)
	add_library(AlsaAudio MODULE src/animations/AlsaAudio.cpp)
	target_include_directories(AlsaAudio PRIVATE ${LIBALSA_INCLUDE_DIRS})
	target_link_libraries(AlsaAudio ledspicer ${LIBALSA_LIBRARIES})
	set_target_properties(AlsaAudio PROPERTIES PREFIX "")
	install(TARGETS AlsaAudio LIBRARY DESTINATION ${ACTORS_DIR})
endif()

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

#################
# Input plugins #
#################

# MAME input plugin
add_plugin(Mame
	"src/inputs/Mame.cpp"
	"${INPUTS_DIR}"
)

# Impulse input plugin
add_plugin(Impulse
	"src/inputs/Impulse.cpp"
	"${INPUTS_DIR}"
)

# Actions input plugin
add_plugin(Actions
	"src/inputs/Actions.cpp"
	"${INPUTS_DIR}"
)

# Blinker input plugin
add_plugin(Blinker
	"src/inputs/Blinker.cpp"
	"${INPUTS_DIR}"
)

# Credits input plugin
add_plugin(Credits
	"src/inputs/Credits.cpp"
	"${INPUTS_DIR}"
)

# Network input plugin
add_plugin(Network
	"src/inputs/Network.cpp"
	"${INPUTS_DIR}"
)

# Documentation and examples.
install(FILES README.md COPYING DESTINATION ${CMAKE_INSTALL_DOCDIR})
install(FILES
	data/21-ledspicer.rules
	data/ledspicer.conf
	DESTINATION ${CMAKE_INSTALL_DOCDIR}/examples
)

# Pkgconfig file.
configure_file(data/ledspicer.pc.in ledspicer.pc @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/ledspicer.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)

# Base service file top part.
file(WRITE ${CMAKE_BINARY_DIR}/ledspicerd.service
	"[Unit]\n"
	"Description=LEDSpicer Daemon\n"
	"After=network.target"
)

# Add PulseAudio or PipeWire dependencies if enabled.
if(ENABLE_PULSEAUDIO)
	file(APPEND ${CMAKE_BINARY_DIR}/ledspicerd.service
		" pulseaudio.service"
	)
endif()

# Continue base service file.
file(APPEND ${CMAKE_BINARY_DIR}/ledspicerd.service
	"\n\n[Service]\n"
	"Type=simple\n"
	"ExecStart=${CMAKE_INSTALL_PREFIX}/bin/ledspicerd\n"
	"Restart=always\n"
)

# Add PulseAudio environment variable if enabled.
if(ENABLE_PULSEAUDIO)
	file(APPEND ${CMAKE_BINARY_DIR}/ledspicerd.service
		"ExecStartPre=/bin/sh -c 'for i in {1..30}; do [ -S /run/user/1000/pulse/native ] && exit 0; sleep 1; done; exit 1'\n"
		"Environment=PULSE_SERVER=unix:/run/user/1000/pulse/native\n"
	)
endif()

# Base service file bottom part.
file(APPEND ${CMAKE_BINARY_DIR}/ledspicerd.service
	"\n[Install]\n"
	"WantedBy=multi-user.target\n"
)

install(FILES ${CMAKE_BINARY_DIR}/ledspicerd.service DESTINATION ${CMAKE_INSTALL_LIBDIR}/systemd/system)

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
Devices dir  : ${DEVICES_DIR}
Actors dir   : ${ACTORS_DIR}
Inputs dir   : ${INPUTS_DIR}
Develop mode : ${ENABLE_DEVELOP}
Dry run mode : ${ENABLE_DRY_RUN}
Run tests    : ${ENABLE_TESTS}
PULSE AUDIO  : ${ENABLE_PULSEAUDIO}
ALSA AUDIO   : ${ENABLE_ALSAAUDIO}
MiSTer code  : ${ENABLE_MISTER}
--------- Output Plugins -----------
NanoLed      : ${ENABLE_NANOLED}
PacDrive     : ${ENABLE_PACDRIVE}
PacLed64     : ${ENABLE_PACLED64}
Ultimate IO  : ${ENABLE_ULTIMATEIO}
LedWiz 32    : ${ENABLE_LEDWIZ32}
Howler       : ${ENABLE_HOWLER}
Raspberry Pi : ${ENABLE_RASPBERRYPI}
Adalight     : ${ENABLE_ADALIGHT}
")