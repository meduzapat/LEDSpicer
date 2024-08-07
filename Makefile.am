
ACLOCAL_AMFLAGS = -I m4

AM_CPPFLAGS =                             \
	-DPACKAGE_CONF_DIR=\"$(sysconfdir)/\" \
	-DPACKAGE_DATA_DIR=\"$(pkgdatadir)/\" \
	-I$(srcdir)/src                       \
	-Wl,--no-as-needed                    \
	-fexceptions

plugins_LDFLAGS =   \
	-module         \
	-shared         \
	-export-dynamic \
	-avoid-version

EXTRA_DIST =

##################
# Shared Library #
##################

pkglib_LTLIBRARIES = \
	libledspicer1.la

libledspicer1_la_SOURCES =    \
	src/utility/Log.cpp       \
	src/utility/Error.cpp     \
	src/utility/Utility.cpp   \
	src/utility/Hardware.cpp  \
	src/utility/Serial.cpp    \
	src/utility/USB.cpp       \
	src/utility/XMLHelper.cpp \
	src/utility/Speed.cpp     \
	src/utility/Direction.cpp \
	src/utility/Color.cpp     \
	src/utility/Colors.cpp    \
	src/utility/Colorful.cpp  \
	src/utility/Socks.cpp     \
	src/utility/Time.cpp      \
	src/utility/Monochromatic.cpp \
	src/restrictors/Restrictor.cpp       \
	src/restrictors/RestrictorUSB.cpp    \
	src/restrictors/RestrictorSerial.cpp \
	src/devices/Element.cpp      \
	src/devices/Group.cpp        \
	src/devices/Profile.cpp      \
	src/devices/Device.cpp       \
	src/devices/DeviceSerial.cpp \
	src/devices/DeviceUSB.cpp    \
	src/animations/Actor.cpp          \
	src/animations/FrameActor.cpp     \
	src/animations/DirectionActor.cpp \
	src/animations/StepActor.cpp      \
	src/animations/AudioActor.cpp     \
	src/inputs/Input.cpp  \
	src/inputs/Reader.cpp \
	src/Message.cpp \
	src/Messages.cpp

libledspicer1_la_LDFLAGS =       \
	-version-info $(LIB_VERSION) \
	$(LIBUSB_LIBS)

libledspicer1_la_CPPFLAGS = \
	$(AM_CPPFLAGS)          \
	$(LIBUSB_CFLAGS)

pkginclude_HEADERS = \
	src/Message.hpp  \
	src/Messages.hpp

utilityheaderdir=$(pkgincludedir)/utility
utilityheader_HEADERS =           \
	src/utility/Log.hpp           \
	src/utility/Error.hpp         \
	src/utility/Hardware.hpp      \
	src/utility/Connection.hpp    \
	src/utility/Serial.hpp        \
	src/utility/USB.hpp           \
	src/utility/Utility.hpp       \
	src/utility/Socks.hpp         \
	src/utility/Time.hpp          \
	src/utility/Monochromatic.hpp \
	src/utility/XMLHelper.hpp     \
	src/utility/Speed.hpp         \
	src/utility/Direction.hpp     \
	src/utility/Color.hpp         \
	src/utility/Colors.hpp        \
	src/utility/Colorful.hpp

restrictorssheaderdir=$(pkgincludedir)/restrictors
restrictorssheader_HEADERS =         \
	src/restrictors/ServoStik.hpp    \
	src/restrictors/UltraStik360.hpp \
	src/restrictors/GPWiz40RotoX.hpp \
	src/restrictors/GPWiz49.hpp      \
	src/restrictors/TOS428.hpp       \
	src/restrictors/Restrictor.hpp   \
	src/restrictors/RestrictorUSB.hpp    \
	src/restrictors/RestrictorSerial.hpp

devicesheaderdir=$(pkgincludedir)/devices
devicesheader_HEADERS =          \
	src/devices/Element.hpp      \
	src/devices/Group.hpp        \
	src/devices/Device.hpp       \
	src/devices/DeviceSerial.hpp \
	src/devices/DeviceUSB.hpp    \
	src/devices/Profile.hpp

animationsheaderdir=$(pkgincludedir)/animations
animationsheader_HEADERS =            \
	src/animations/AudioActor.hpp     \
	src/animations/Actor.hpp          \
	src/animations/FrameActor.hpp     \
	src/animations/DirectionActor.hpp \
	src/animations/StepActor.hpp

inputsheaderdir=$(pkgincludedir)/inputs
inputsheader_HEADERS =    \
	src/inputs/Input.hpp  \
	src/inputs/Reader.hpp

ledspicer_pcdir = \
	$(pkgconfigdir)

ledspicer_pc_DATA = \
	data/ledspicer.pc

##############
# ledspicerd #
##############

bin_PROGRAMS =   \
	ledspicerd   \
	emitter      \
	inputseeker  \
	rotator      \
	processLookup

ledspicerd_SOURCES =                \
	src/Handler.cpp                 \
	src/animations/ActorHandler.cpp \
	src/devices/DeviceHandler.cpp   \
	src/inputs/InputHandler.cpp     \
	src/DataLoader.cpp              \
	src/MainBase.cpp                \
	src/Main.cpp

ledspicerd_CPPFLAGS =                 \
	$(AM_CPPFLAGS)                    \
	-DACTORS_DIR=\"$(ACTORS_DIR)/\"   \
	-DDEVICES_DIR=\"$(DEVICES_DIR)/\" \
	-DINPUTS_DIR=\"$(INPUTS_DIR)/\"   \
	$(TINYXML2_CFLAGS)                \
	$(PTHREADSTUBS_CFLAGS)            \
	$(LINKER_CFLAGS)                  \
	$(LIBUSB_CFLAGS)

ledspicerd_LDFLAGS =     \
	$(TINYXML2_LIBS)     \
	$(PTHREADSTUBS_LIBS) \
	$(LINKER_LIBS)       \
	$(LIBUSB_LIBS)

ledspicerd_LDADD = \
	libledspicer1.la

###########
# emitter #
###########

emitter_SOURCES =   \
	src/Emitter.cpp

emitter_CPPFLAGS =     \
	$(AM_CPPFLAGS)     \
	$(TINYXML2_CFLAGS)

emitter_LDFLAGS =    \
	$(TINYXML2_LIBS)

emitter_LDADD = \
	libledspicer1.la

###########
# rotator #
###########

rotator_SOURCES =                    \
	src/restrictors/ServoStik.cpp    \
	src/restrictors/UltraStik360.cpp \
	src/restrictors/GPWiz49.cpp      \
	src/restrictors/GPWiz40RotoX.cpp \
	src/restrictors/TOS428.cpp       \
	src/Rotator.cpp

rotator_CPPFLAGS =     \
	$(AM_CPPFLAGS)     \
	$(TINYXML2_CFLAGS) \
	$(LIBUSB_CFLAGS)

rotator_LDFLAGS =    \
	$(TINYXML2_LIBS) \
	$(LIBUSB_LIBS)

rotator_LDADD = \
	libledspicer1.la

###############
# inputseeker #
###############

inputseeker_SOURCES = \
	src/InputSeeker.cpp

#################
# processLookup #
#################

processLookup_SOURCES = \
	src/ProcessLookup.cpp

processLookup_CPPFLAGS = \
	$(AM_CPPFLAGS)       \
	$(TINYXML2_CFLAGS)

processLookup_LDFLAGS = \
	$(TINYXML2_LIBS)

processLookup_LDADD = \
	libledspicer1.la

##########
# actors #
##########

actors_LTLIBRARIES = \
	Gradient.la      \
	Pulse.la         \
	Serpentine.la    \
	Random.la        \
	Filler.la        \
	FileReader.la

actorsdir = \
	$(ACTORS_DIR)

############
# Gradient #
############

Gradient_la_SOURCES = \
	src/animations/Gradient.cpp

Gradient_la_LIBADD = \
	libledspicer1.la

Gradient_la_LDFLAGS = \
	$(plugins_LDFLAGS)

#########
# Pulse #
#########

Pulse_la_SOURCES = \
	src/animations/Pulse.cpp

Pulse_la_LIBADD = \
	libledspicer1.la

Pulse_la_LDFLAGS = \
	$(plugins_LDFLAGS)

##############
# Serpentine #
##############

Serpentine_la_SOURCES = \
	src/animations/Serpentine.cpp

Serpentine_la_LIBADD = \
	libledspicer1.la

Serpentine_la_LDFLAGS = \
	$(plugins_LDFLAGS)

##########
# Random #
##########

Random_la_SOURCES = \
	src/animations/Random.cpp

Random_la_LIBADD = \
	libledspicer1.la

Random_la_LDFLAGS = \
	$(plugins_LDFLAGS)

##########
# Filler #
##########

Filler_la_SOURCES = \
	src/animations/Filler.cpp

Filler_la_LIBADD = \
	libledspicer1.la

Filler_la_LDFLAGS = \
	$(plugins_LDFLAGS)

##############
# PulseAudio #
##############

if PULSEAUDIO
actors_LTLIBRARIES += \
	PulseAudio.la

PulseAudio_la_SOURCES = \
	src/animations/PulseAudio.cpp

PulseAudio_la_LIBADD = \
	libledspicer1.la   \
	$(LIBPULSE_LIBS)

PulseAudio_la_CPPFLAGS = \
	$(AM_CPPFLAGS)       \
	$(LIBPULSE_CFLAGS)

PulseAudio_la_LDFLAGS = \
	$(plugins_LDFLAGS)

else
EXTRA_DIST += \
	src/animations/PulseAudio.cpp
endif

#############
# AlsaAudio #
#############

if ALSAAUDIO
actors_LTLIBRARIES += \
	AlsaAudio.la

AlsaAudio_la_SOURCES = \
	src/animations/AlsaAudio.cpp

AlsaAudio_la_LIBADD = \
	libledspicer1.la   \
	$(LIBALSA_LIBS)

AlsaAudio_la_CPPFLAGS = \
	$(AM_CPPFLAGS)       \
	$(LIBALSA_CFLAGS)

AlsaAudio_la_LDFLAGS = \
	$(plugins_LDFLAGS)

else
EXTRA_DIST += \
	src/animations/AlsaAudio.cpp
endif

##############
# FileReader #
##############

FileReader_la_SOURCES = \
	src/animations/FileReader.cpp

FileReader_la_LIBADD = \
	libledspicer1.la

FileReader_la_LDFLAGS = \
	$(plugins_LDFLAGS)

###########
# devices #
###########

devices_CPPFLAGS =  \
	$(AM_CPPFLAGS)  \
	$(LIBUSB_CFLAGS)

devices_LTLIBRARIES =

####################
# Ultimarc NanoLed #
####################
if NANOLED

devices_LTLIBRARIES += \
	UltimarcNanoLed.la

UltimarcNanoLed_la_SOURCES =                \
	src/devices/Ultimarc/FF00SharedCode.cpp \
	src/devices/Ultimarc/NanoLed.cpp

UltimarcNanoLed_la_LIBADD = \
	libledspicer1.la

UltimarcNanoLed_la_LDFLAGS = \
	$(plugins_LDFLAGS)

UltimarcNanoLed_la_CPPFLAGS = \
	$(devices_CPPFLAGS)

else
EXTRA_DIST += \
	src/devices/Ultimarc/NanoLed.cpp
endif

#####################
# Ultimarc PacDrive #
#####################
if PACDRIVE
devices_LTLIBRARIES += \
	UltimarcPacDrive.la

UltimarcPacDrive_la_SOURCES = \
	src/devices/Ultimarc/PacDrive.cpp

UltimarcPacDrive_la_LIBADD = \
	libledspicer1.la

UltimarcPacDrive_la_LDFLAGS = \
	$(plugins_LDFLAGS)

UltimarcPacDrive_la_CPPFLAGS = \
	$(devices_CPPFLAGS)

else
EXTRA_DIST += \
	src/devices/Ultimarc/PacDrive.cpp
endif

#####################
# Ultimarc PacLed64 #
#####################
if PACLED64
devices_LTLIBRARIES += \
	UltimarcPacLed64.la

UltimarcPacLed64_la_SOURCES =               \
	src/devices/Ultimarc/FF00SharedCode.cpp \
	src/devices/Ultimarc/PacLed64.cpp

UltimarcPacLed64_la_LIBADD = \
	libledspicer1.la

UltimarcPacLed64_la_LDFLAGS = \
	$(plugins_LDFLAGS)

UltimarcPacLed64_la_CPPFLAGS = \
	$(devices_CPPFLAGS)

else
EXTRA_DIST += \
	src/devices/Ultimarc/PacLed64.cpp
endif

#####################
# Ultimarc Ultimate #
#####################
if ULTIMATEIO
devices_LTLIBRARIES += \
	UltimarcUltimate.la

UltimarcUltimate_la_SOURCES = \
	src/devices/Ultimarc/Ultimate.cpp

UltimarcUltimate_la_LIBADD = \
	libledspicer1.la

UltimarcUltimate_la_LDFLAGS = \
	$(plugins_LDFLAGS)

UltimarcUltimate_la_CPPFLAGS = \
	$(devices_CPPFLAGS)

else
EXTRA_DIST += \
	src/devices/Ultimarc/Ultimate.cpp
endif

#############
# LEDWiz 32 #
#############
if LEDWIZ32
devices_LTLIBRARIES += \
	LedWiz32.la

LedWiz32_la_SOURCES = \
	src/devices/GroovyGameGear/LedWiz32.cpp

LedWiz32_la_LIBADD = \
	libledspicer1.la

LedWiz32_la_LDFLAGS = \
	$(plugins_LDFLAGS)

LedWiz32_la_CPPFLAGS = \
	$(devices_CPPFLAGS)

else
EXTRA_DIST += \
	src/devices/GroovyGameGear/LedWiz32.cpp
endif

##########
# HOWLER #
##########
if HOWLER
devices_LTLIBRARIES += \
	Howler.la

Howler_la_SOURCES = \
	src/devices/WolfWareTech/Howler.cpp

Howler_la_LIBADD = \
	libledspicer1.la

Howler_la_LDFLAGS = \
	$(plugins_LDFLAGS)

Howler_la_CPPFLAGS = \
	$(devices_CPPFLAGS)

else
EXTRA_DIST += \
	src/devices/WolfWareTech/Howler.cpp
endif

###############
# RASPBERRYPI #
###############
if RASPBERRYPI
devices_LTLIBRARIES += \
	RaspberryPi.la

RaspberryPi_la_SOURCES = \
	src/devices/RaspberryPiGPIO/RaspberryPi.cpp

RaspberryPi_la_LIBADD = \
	libledspicer1.la    \
	$(RASPBERRYPI_LIBS)

RaspberryPi_la_LDFLAGS = \
	$(plugins_LDFLAGS)

RaspberryPi_la_CPPFLAGS = \
	$(devices_CPPFLAGS)

else
EXTRA_DIST += \
	src/devices/RaspberryPiGPIO/RaspberryPi.cpp
endif

devicesdir = \
	$(DEVICES_DIR)

###############
#   ADALIGHT  #
###############
if ADALIGHT
devices_LTLIBRARIES += \
	Adalight.la

Adalight_la_SOURCES = \
	src/devices/Adalight/Adalight.cpp

Adalight_la_LIBADD = \
	libledspicer1.la \
	$(ADALIGHT_LIBS)

Adalight_la_LDFLAGS = \
	$(plugins_LDFLAGS)

Adalight_la_CPPFLAGS = \
	$(devices_CPPFLAGS)

else
EXTRA_DIST += \
	src/devices/Adalight/Adalight.cpp
endif


##########
# inputs #
##########

inputsdir = \
	$(INPUTS_DIR)

inputs_LTLIBRARIES = \
	Mame.la          \
	Impulse.la       \
	Actions.la       \
	Blinker.la       \
	Credits.la       \
	Network.la

########
# MAME #
########

Mame_la_SOURCES = \
	src/inputs/Mame.cpp

Mame_la_LIBADD = \
	libledspicer1.la

Mame_la_LDFLAGS = \
	$(plugins_LDFLAGS)

###########
# IMPULSE #
###########

Impulse_la_SOURCES = \
	src/inputs/Impulse.cpp

Impulse_la_LIBADD = \
	libledspicer1.la

Impulse_la_LDFLAGS = \
	$(plugins_LDFLAGS)

###########
# Actions #
###########

Actions_la_SOURCES = \
	src/inputs/Actions.cpp

Actions_la_LIBADD = \
	libledspicer1.la

Actions_la_LDFLAGS = \
	$(plugins_LDFLAGS)

###########
# Blinker #
###########

Blinker_la_SOURCES = \
	src/inputs/Blinker.cpp

Blinker_la_LIBADD = \
	libledspicer1.la

Blinker_la_LDFLAGS = \
	$(plugins_LDFLAGS)

###########
# Credits #
###########

Credits_la_SOURCES = \
	src/inputs/Credits.cpp

Credits_la_LIBADD = \
	libledspicer1.la

Credits_la_LDFLAGS = \
	$(plugins_LDFLAGS)

###########
# Network #
###########

Network_la_SOURCES = \
	src/inputs/Network.cpp

Network_la_LIBADD = \
	libledspicer1.la

Network_la_LDFLAGS = \
	$(plugins_LDFLAGS)

######################
# Ultrastik 360 maps #
######################

mapsdatadir=$(pkgdatadir)/umaps
mapsdata_DATA =         \
	data/maps/2Way.um   \
	data/maps/2WayV.um  \
	data/maps/4Way.um   \
	data/maps/4WayX.um  \
	data/maps/8Way.um   \
	data/maps/Analog.um \
	data/maps/Mouse.um

################################
# Devices, Profiles and Inputs #
################################

animations_datadir=$(pkgdatadir)/animations
animations_data_DATA = \
	data/animations/readme.txt

profiles_datadir=$(pkgdatadir)/profiles
profiles_data_DATA =         \
	data/profiles/readme.txt

inputs_datadir=$(pkgdatadir)/inputs
inputs_data_DATA = \
	data/inputs/readme.txt

#############################
# documentation and samples #
#############################

dist_doc_DATA = \
	README      \
	COPYING

examplesdir=$(docdir)/examples
examples_DATA =             \
	data/21-ledspicer.rules \
	data/ledspicerd.service \
	data/ledspicer.conf     \
	data/animations/animation-skeleton.xml \
	data/profiles/profile-skeleton.xml     \
	data/profiles/profile_empty.xml        \
	data/inputs/input-skeleton.xml

######################
# extra distribution #
######################

DISTCLEANFILES = \
	$(ledspicer_pc_DATA)

EXTRA_DIST +=                       \
	autogen.sh                      \
	src/Brands.hpp                  \
	src/Handler.hpp                 \
	src/animations/ActorHandler.hpp \
	src/devices/DeviceHandler.hpp   \
	src/inputs/InputHandler.hpp     \
	src/DataLoader.hpp              \
	src/devices/GroovyGameGear/GroovyGameGear.hpp \
	src/devices/GroovyGameGear/LedWiz32.hpp \
	src/devices/Ultimarc/FF00SharedCode.hpp \
	src/devices/Ultimarc/FF00SharedCode.cpp \
	src/devices/Ultimarc/Ultimarc.hpp \
	src/devices/Ultimarc/Ultimate.hpp \
	src/devices/Ultimarc/PacDrive.hpp \
	src/devices/Ultimarc/PacLed64.hpp \
	src/devices/Ultimarc/NanoLed.hpp  \
	src/devices/WolfWareTech/Howler.hpp         \
	src/devices/WolfWareTech/WolfWareTech.hpp   \
	src/devices/RaspberryPiGPIO/RaspberryPi.hpp \
	src/devices/Adalight/Adalight.hpp \
	src/animations/Gradient.hpp   \
	src/animations/Pulse.hpp      \
	src/animations/Serpentine.hpp \
	src/animations/Random.hpp     \
	src/animations/Filler.hpp     \
	src/animations/AudioActor.hpp \
	src/animations/PulseAudio.hpp \
	src/animations/AlsaAudio.hpp  \
	src/animations/FileReader.hpp \
	src/inputs/Mame.hpp           \
	src/inputs/Reader.hpp         \
	src/inputs/Impulse.hpp        \
	src/inputs/Actions.hpp        \
	src/inputs/Blinker.hpp        \
	src/inputs/Credits.hpp        \
	src/inputs/Network.hpp        \
	src/MainBase.hpp      \
	src/Main.hpp          \
	src/Emitter.hpp       \
	src/Rotator.hpp       \
	src/ProcessLookup.hpp \
	src/InputSeeker.hpp   \
	data/ledspicer.conf        \
	data/animations/readme.txt \
	data/profiles/readme.txt   \
	data/inputs/readme.txt     \
	data/21-ledspicer.rules    \
	data/ledspicerd.service    \
	data/profiles/profile_empty.xml        \
	data/animations/animation-skeleton.xml \
	data/profiles/profile-skeleton.xml     \
	data/inputs/input-skeleton.xml         \
	data/maps/2Way.um   \
	data/maps/2WayV.um  \
	data/maps/4Way.um   \
	data/maps/4WayX.um  \
	data/maps/8Way.um   \
	data/maps/Analog.um \
	data/maps/Mouse.um  \
	AUTHORS             \
	ChangeLog           \
	NEWS
