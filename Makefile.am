
ACLOCAL_AMFLAGS = -I m4

AM_CPPFLAGS =          \
	-I$(top_srcdir)/src\
	${LIBUSB_CFLAGS}

AM_LDFLAGS = \
	${LIBUSB_LIBS}

##################
# Shared Library #
##################

pkglib_LTLIBRARIES = \
	libledspicer.la

libledspicer_la_SOURCES = \
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

pkginclude_HEADERS = \
	src/ConnectionUSB.hpp         \
	src/Message.hpp               \
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
	-DACTORS_DIR=\"$(ACTORS_DIR)/\"        \
	-DDEVICES_DIR=\"$(DEVICES_DIR)/\"      \
	${TINYXML2_CFLAGS}                    \
	${PTHREADSTUBS_CFLAGS}                \
	${LINKER_CFLAGS}                      \
	${LIBUSB_CFLAGS}

ledspicerd_CXXFLAGS =   \
	-I$(top_srcdir)/src \
	${CXXFLAGS}         \
	-Wl,--no-as-needed

ledspicerd_LDFLAGS =     \
	${TINYXML2_LIBS}     \
	${PTHREADSTUBS_LIBS} \
	${LINKER_LIBS}       \
	${LIBUSB_LIBS}

ledspicerd_LDADD = \
	libledspicer.la

###########
# emitter #
###########

emitter_SOURCES = \
	src/Emitter.cpp

emitter_CPPFLAGS =    \
	-DPACKAGE_CONF_DIR=\"$(sysconfdir)\" \
	${TINYXML2_CFLAGS}

emitter_CXXFLAGS =      \
	-I$(top_srcdir)/src \
	${CXXFLAGS}         \
	-Wl,--no-as-needed

emitter_LDFLAGS = \
	${TINYXML2_LIBS}

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
	-avoid-version

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
	-avoid-version

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
	-avoid-version

##########
# Random #
##########

Random_la_SOURCES = \
	src/animations/Random.cpp

Random_la_LIBADD = \
	libledspicer.la

Random_la_LDFLAGS =    \
	-module         \
	-shared         \
	-export-dynamic \
	-avoid-version

##########
# Filler #
##########

Filler_la_SOURCES = \
	src/animations/Filler.cpp

Filler_la_LIBADD = \
	libledspicer.la

Filler_la_LDFLAGS =    \
	-module         \
	-shared         \
	-export-dynamic \
	-avoid-version

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
	-module                   \
	-shared                   \
	-export-dynamic           \
	-avoid-version

#####################
# Ultimarc PacDrive #
#####################

UltimarcPacDrive_la_SOURCES = \
	src/devices/Ultimarc/PacDrive.cpp

UltimarcPacDrive_la_LIBADD = \
	libledspicer.la

UltimarcPacDrive_la_LDFLAGS = \
	-module                   \
	-shared                   \
	-export-dynamic           \
	-avoid-version

##########
# LedWiz #
##########

LedWiz32_la_SOURCES = \
	src/devices/LedWiz/LedWiz32.cpp

LedWiz32_la_LIBADD = \
	libledspicer.la

LedWiz32_la_LDFLAGS =  \
	-module            \
	-shared            \
	-export-dynamic    \
	-avoid-version

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

DISTCLEANFILES = \
	$(systemdsystemunit_DATA)

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

EXTRA_DIST =                          \
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
