
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
	libledspicer.la

libledspicer_la_SOURCES =         \
	src/utility/Log.cpp           \
	src/utility/Error.cpp         \
	src/utility/Utility.cpp       \
	src/utility/XMLHelper.cpp     \
	src/utility/Speed.cpp         \
	src/utility/Direction.cpp     \
	src/utility/Color.cpp         \
	src/devices/Element.cpp       \
	src/devices/Group.cpp         \
	src/ConnectionUSB.cpp         \
	src/devices/Device.cpp        \
	src/animations/Actor.cpp      \
	src/animations/TimedActor.cpp \
	src/inputs/Input.cpp          \
	src/inputs/Reader.cpp         \
	src/devices/Profile.cpp       \
	src/utility/Socks.cpp         \
	src/Message.cpp               \
	src/Messages.cpp

libledspicer_la_LDFLAGS =        \
	-version-info $(LIB_VERSION) \
	$(LIBUSB_LIBS)

libledspicer_la_CPPFLAGS = \
	$(AM_CPPFLAGS)         \
	$(LIBUSB_CFLAGS)

pkginclude_HEADERS =      \
	src/ConnectionUSB.hpp \
	src/Message.hpp       \
	src/Messages.hpp

utilityheaderdir=$(pkgincludedir)/utility
utilityheader_HEADERS =       \
	src/utility/Log.hpp       \
	src/utility/Error.hpp     \
	src/utility/Utility.hpp   \
	src/utility/Socks.hpp     \
	src/utility/XMLHelper.hpp \
	src/utility/Speed.hpp     \
	src/utility/Direction.hpp \
	src/utility/Color.hpp

devicesheaderdir=$(pkgincludedir)/devices
devicesheader_HEADERS =     \
	src/devices/Element.hpp \
	src/devices/Group.hpp   \
	src/devices/Device.hpp  \
	src/devices/Profile.hpp

animationsheaderdir=$(pkgincludedir)/animations
animationsheader_HEADERS =        \
	src/animations/Actor.hpp      \
	src/animations/TimedActor.hpp

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

bin_PROGRAMS = \
	ledspicerd \
	emitter    \
	inputseeker

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
	libledspicer.la

###########
# emitter #
###########

emitter_SOURCES = \
	src/Emitter.cpp

emitter_CPPFLAGS =    \
	$(AM_CPPFLAGS)    \
	$(TINYXML2_CFLAGS)

emitter_LDFLAGS = \
	$(TINYXML2_LIBS)

emitter_LDADD = \
	libledspicer.la

###############
# inputseeker #
###############

inputseeker_SOURCES = \
	src/InputSeeker.cpp

##########
# actors #
##########

actors_LTLIBRARIES = \
	Gradient.la      \
	Pulse.la         \
	Serpentine.la    \
	Random.la        \
	Filler.la

actorsdir = \
	$(ACTORS_DIR)

############
# Gradient #
############

Gradient_la_SOURCES = \
	src/animations/Gradient.cpp

Gradient_la_LIBADD = \
	libledspicer.la

Gradient_la_LDFLAGS = \
	$(plugins_LDFLAGS)

#########
# Pulse #
#########

Pulse_la_SOURCES = \
	src/animations/Pulse.cpp

Pulse_la_LIBADD = \
	libledspicer.la

Pulse_la_LDFLAGS = \
	$(plugins_LDFLAGS)

##############
# Serpentine #
##############

Serpentine_la_SOURCES = \
	src/animations/Serpentine.cpp

Serpentine_la_LIBADD = \
	libledspicer.la

Serpentine_la_LDFLAGS = \
	$(plugins_LDFLAGS)

##########
# Random #
##########

Random_la_SOURCES = \
	src/animations/Random.cpp

Random_la_LIBADD = \
	libledspicer.la

Random_la_LDFLAGS = \
	$(plugins_LDFLAGS)

##########
# Filler #
##########

Filler_la_SOURCES = \
	src/animations/Filler.cpp

Filler_la_LIBADD = \
	libledspicer.la

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
	libledspicer.la    \
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
	libledspicer.la

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
	libledspicer.la

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
	libledspicer.la

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
	libledspicer.la

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
	libledspicer.la

LedWiz32_la_LDFLAGS = \
	$(plugins_LDFLAGS)

LedWiz32_la_CPPFLAGS = \
	$(devices_CPPFLAGS)

else
EXTRA_DIST += \
	src/devices/GroovyGameGear/LedWiz32.cpp
endif

devicesdir = \
	$(DEVICES_DIR)

##########
# inputs #
##########

inputsdir = \
	$(INPUTS_DIR)

inputs_LTLIBRARIES = \
	Mame.la          \
	Impulse.la       \
	Actions.la       \
	Blinker.la

########
# MAME #
########

Mame_la_SOURCES = \
	src/inputs/Mame.cpp

Mame_la_LIBADD = \
	libledspicer.la

Mame_la_LDFLAGS = \
	$(plugins_LDFLAGS)

###########
# IMPULSE #
###########

Impulse_la_SOURCES = \
	src/inputs/Impulse.cpp

Impulse_la_LIBADD = \
	libledspicer.la

Impulse_la_LDFLAGS = \
	$(plugins_LDFLAGS)

###########
# Actions #
###########

Actions_la_SOURCES = \
	src/inputs/Actions.cpp

Actions_la_LIBADD = \
	libledspicer.la

Actions_la_LDFLAGS = \
	$(plugins_LDFLAGS)

###########
# Blinker #
###########

Blinker_la_SOURCES = \
	src/inputs/Blinker.cpp

Blinker_la_LIBADD = \
	libledspicer.la

Blinker_la_LDFLAGS = \
	$(plugins_LDFLAGS)

##############
# games data #
##############

gamesdatadir=$(pkgdatadir)
gamesdata_DATA = \
	data/gameData.xml

if COPY_SAMPLES

##########
# config #
##########

ledspicerdconfdir=$(sysconfdir)
ledspicerdconf_DATA = \
	data/ledspicer.conf

###########
# samples #
###########

sampledir=$(pkgdatadir)
sample_DATA =            \
	data/basicColors.xml \
	data/webColors.xml

sample_animationsdir=$(pkgdatadir)/animations
sample_animations_DATA =        \
	data/animations/example.xml \
	data/animations/skeleton.xml

sample_profilesdir=$(pkgdatadir)/profiles
sample_profiles_DATA =        \
	data/profiles/default.xml \
	data/profiles/skeleton.xml

endif

###########
# systemd #
###########

AM_DISTCHECK_CONFIGURE_FLAGS = \
	--with-systemdsystemunitdir='$$(prefix)/$(systemdsystemunitdir)'

systemdsystemunit_DATA = \
	data/ledspicerd.service

#################
# documentation #
#################

dist_doc_DATA =             \
	README                  \
	COPYING                 \
	data/21-ledspicer.rules

######################
# extra distribution #
######################

DISTCLEANFILES =              \
	$(systemdsystemunit_DATA) \
	$(ledspicer_pc_DATA)

EXTRA_DIST +=                       \
	autogen.sh                      \
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
	src/animations/Gradient.hpp   \
	src/animations/Pulse.hpp      \
	src/animations/Serpentine.hpp \
	src/animations/Random.hpp     \
	src/animations/Filler.hpp     \
	src/animations/PulseAudio.hpp \
	src/inputs/Mame.hpp           \
	src/inputs/Reader.hpp         \
	src/inputs/Impulse.hpp        \
	src/inputs/Actions.hpp        \
	src/inputs/Blinker.hpp        \
	src/MainBase.hpp     \
	src/Main.hpp         \
	src/Emitter.hpp      \
	src/InputSeeker.hpp  \
	data/ledspicer.conf          \
	data/basicColors.xml         \
	data/webColors.xml           \
	data/gameData.xml            \
	data/animations/example.xml  \
	data/animations/skeleton.xml \
	data/profiles/default.xml    \
	data/profiles/skeleton.xml   \
	AUTHORS                      \
	ChangeLog                    \
	NEWS
