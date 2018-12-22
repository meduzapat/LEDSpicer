
ACLOCAL_AMFLAGS = -I m4

AM_CPPFLAGS =          \
	-I$(srcdir)/src    \
	-Wl,--no-as-needed \
	-fexceptions       \
	$(LIBUSB_CFLAGS)

AM_LDFLAGS =                     \
	-version-info $(LIB_VERSION) \
	$(LIBUSB_LIBS)

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
	src/utility/Color.cpp         \
	src/devices/Element.cpp       \
	src/devices/Group.cpp         \
	src/ConnectionUSB.cpp         \
	src/devices/Device.cpp        \
	src/animations/Actor.cpp      \
	src/animations/TimedActor.cpp \
	src/devices/Profile.cpp       \
	src/utility/Socks.cpp         \
	src/Message.cpp               \
	src/Messages.cpp

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

ledspicer_pcdir = \
	$(pkgconfigdir)

ledspicer_pc_DATA = \
	data/ledspicer.pc

##############
# ledspicerd #
##############

bin_PROGRAMS = \
	ledspicerd \
	emitter

ledspicerd_SOURCES =                \
	src/Handler.cpp                 \
	src/animations/ActorHandler.cpp \
	src/devices/DeviceHandler.cpp   \
	src/DataLoader.cpp              \
	src/MainBase.cpp                \
	src/Main.cpp

ledspicerd_CPPFLAGS =                     \
	-DPACKAGE_CONF_DIR=\"$(sysconfdir)/\" \
	-DPACKAGE_DATA_DIR=\"$(pkgdatadir)/\" \
	-DACTORS_DIR=\"$(ACTORS_DIR)/\"       \
	-DDEVICES_DIR=\"$(DEVICES_DIR)/\"     \
	$(TINYXML2_CFLAGS)                    \
	$(PTHREADSTUBS_CFLAGS)                \
	$(LINKER_CFLAGS)                      \
	$(LIBUSB_CFLAGS)                      \
	-I$(top_srcdir)/src                   \
	-Wl,--no-as-needed                    \
	-fexceptions

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

emitter_CPPFLAGS =                       \
	-DPACKAGE_CONF_DIR=\"$(sysconfdir)\" \
	$(TINYXML2_CFLAGS)                   \
	-I$(top_srcdir)/src                  \
	-Wl,--no-as-needed

emitter_LDFLAGS = \
	$(TINYXML2_LIBS)

emitter_LDADD = \
	libledspicer.la

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
	-module           \
	-shared           \
	-export-dynamic   \
	-avoid-version    \
	-fexceptions

#########
# Pulse #
#########

Pulse_la_SOURCES = \
	src/animations/Pulse.cpp

Pulse_la_LIBADD = \
	libledspicer.la

Pulse_la_LDFLAGS =  \
	-module         \
	-shared         \
	-export-dynamic \
	-avoid-version  \
	-fexceptions

##############
# Serpentine #
##############

Serpentine_la_SOURCES = \
	src/animations/Serpentine.cpp

Serpentine_la_LIBADD = \
	libledspicer.la

Serpentine_la_LDFLAGS = \
	-module             \
	-shared             \
	-export-dynamic     \
	-avoid-version      \
	-fexceptions

##########
# Random #
##########

Random_la_SOURCES = \
	src/animations/Random.cpp

Random_la_LIBADD = \
	libledspicer.la

Random_la_LDFLAGS = \
	-module         \
	-shared         \
	-export-dynamic \
	-avoid-version  \
	-fexceptions

##########
# Filler #
##########

Filler_la_SOURCES = \
	src/animations/Filler.cpp

Filler_la_LIBADD = \
	libledspicer.la

Filler_la_LDFLAGS = \
	-module         \
	-shared         \
	-export-dynamic \
	-avoid-version  \
	-fexceptions

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

PulseAudio_la_LDFLAGS = \
	$(LIBPULSE)         \
	-module             \
	-shared             \
	-export-dynamic     \
	-avoid-version      \
	-fexceptions
else
EXTRA_DIST += \
	src/animations/PulseAudio.cpp
endif

###########
# devices #
###########

devices_LTLIBRARIES =   \
	UltimarcUltimate.la \
	UltimarcPacDrive.la \
	LedWiz32.la

devicesdir = \
	$(DEVICES_DIR)

#####################
# Ultimarc Ultimate #
#####################

UltimarcUltimate_la_SOURCES = \
	src/devices/Ultimarc/Ultimate.cpp

UltimarcUltimate_la_LIBADD = \
	libledspicer.la

UltimarcUltimate_la_LDFLAGS = \
	-module         \
	-shared         \
	-export-dynamic \
	-avoid-version  \
	-fexceptions

#####################
# Ultimarc PacDrive #
#####################

UltimarcPacDrive_la_SOURCES = \
	src/devices/Ultimarc/PacDrive.cpp

UltimarcPacDrive_la_LIBADD = \
	libledspicer.la

UltimarcPacDrive_la_LDFLAGS = \
	-module         \
	-shared         \
	-export-dynamic \
	-avoid-version  \
	-fexceptions

##########
# LedWiz #
##########

LedWiz32_la_SOURCES = \
	src/devices/LedWiz/LedWiz32.cpp

LedWiz32_la_LIBADD = \
	libledspicer.la

LedWiz32_la_LDFLAGS = \
	-module           \
	-shared           \
	-export-dynamic   \
	-avoid-version    \
	-fexceptions

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

systemdsystemunit_DATA =    \
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

EXTRA_DIST +=                         \
	autogen.sh                        \
	src/Handler.hpp                   \
	src/animations/ActorHandler.hpp   \
	src/devices/DeviceHandler.hpp     \
	src/DataLoader.hpp                \
	src/devices/LedWiz/LedWiz.hpp     \
	src/devices/LedWiz/LedWiz32.hpp   \
	src/devices/Ultimarc/Ultimarc.hpp \
	src/devices/Ultimarc/Ultimate.hpp \
	src/devices/Ultimarc/PacDrive.hpp \
	src/animations/Gradient.hpp       \
	src/animations/Pulse.hpp          \
	src/animations/Serpentine.hpp     \
	src/animations/Random.hpp         \
	src/animations/Filler.hpp         \
	src/animations/PulseAudio.hpp     \
	src/MainBase.hpp                  \
	src/Main.hpp                      \
	src/Emitter.hpp                   \
	data/ledspicer.conf               \
	data/basicColors.xml              \
	data/webColors.xml                \
	data/animations/example.xml       \
	data/animations/skeleton.xml      \
	data/profiles/default.xml         \
	data/profiles/skeleton.xml        \
	AUTHORS                           \
	ChangeLog                         \
	NEWS
