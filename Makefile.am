
bin_PROGRAMS = \
	ledspicerd \
	emitter

ledspicerd_SOURCES =                  \
	src/utility/Log.cpp               \
	src/utility/Error.cpp             \
	src/utility/Utility.cpp           \
	src/utility/Socks.cpp             \
	src/utility/XMLHelper.cpp         \
	src/utility/Color.cpp             \
	src/DataLoader.cpp                \
	src/ConnectionUSB.cpp             \
	src/devices/Element.cpp           \
	src/devices/Group.cpp             \
	src/devices/Device.cpp            \
	src/devices/LedWiz/LedWiz32.cpp   \
	src/devices/Ultimarc/Ultimate.cpp \
	src/devices/Ultimarc/PacDrive.cpp \
	src/animations/Actor.cpp          \
	src/animations/TimedActor.cpp     \
	src/animations/Gradient.cpp       \
	src/animations/Pulse.cpp          \
	src/animations/Serpentine.cpp     \
	src/animations/Random.cpp         \
	src/animations/Filler.cpp         \
	src/devices/Profile.cpp           \
	src/Messages.cpp                  \
	src/Main.cpp

ledspicerd_CPPFLAGS =                     \
	-DPACKAGE_CONF_DIR=\"$(sysconfdir)\"  \
	-DPACKAGE_SRC_DIR=\"$(srcdir)\"       \
	-DPACKAGE_DATA_DIR=\"$(pkgdatadir)\"  \
	$(LEDSPICER_CPPFLAGS)

ledspicerd_CXXFLAGS =    \
	${CXXFLAGS}          \
	-Wl,--no-as-needed

ledspicerd_LDFLAGS =    \
	${LEDSPICER_LIBS}

emitter_SOURCES =           \
	src/utility/Log.cpp     \
	src/utility/Error.cpp   \
	src/utility/Utility.cpp \
	src/utility/Socks.cpp   \
	src/Emitter.cpp

emitter_CXXFLAGS =     \
	${CXXFLAGS}        \
	-Wl,--no-as-needed

emitter_LDFLAGS =    \
	${LEDSPICER_LIBS}

ledspicerdconfdir=$(sysconfdir)
ledspicerdconf_DATA = \
	data/ledspicerd.conf

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

AM_DISTCHECK_CONFIGURE_FLAGS = \
	--with-systemdsystemunitdir='$$(prefix)/$(systemdsystemunitdir)'

systemdsystemunit_DATA =    \
	data/ledspicerd.service

DISTCLEANFILES = \
	$(systemdsystemunit_DATA)

dist_doc_DATA = \
	README      \
	COPYING

EXTRA_DIST =                          \
	autogen.sh                        \
	src/utility/Log.hpp               \
	src/utility/Error.hpp             \
	src/utility/Utility.hpp           \
	src/utility/Socks.hpp             \
	src/utility/XMLHelper.hpp         \
	src/utility/Color.hpp             \
	src/DataLoader.hpp                \
	src/ConnectionUSB.hpp             \
	src/devices/Element.hpp           \
	src/devices/Group.hpp             \
	src/devices/Device.hpp            \
	src/devices/LedWiz/LedWiz.hpp     \
	src/devices/LedWiz/LedWiz32.hpp   \
	src/devices/Ultimarc/Ultimarc.hpp \
	src/devices/Ultimarc/Ultimate.hpp \
	src/devices/Ultimarc/PacDrive.hpp \
	src/animations/Actor.hpp          \
	src/animations/TimedActor.hpp     \
	src/animations/Gradient.hpp       \
	src/animations/Pulse.hpp          \
	src/animations/Serpentine.hpp     \
	src/animations/Random.hpp         \
	src/animations/Filler.hpp         \
	src/devices/Profile.hpp           \
	src/Messages.hpp                  \
	src/Message.hpp                   \
	src/Main.hpp                      \
	src/Emitter.hpp                   \
	data/ledspicerd.conf              \
	data/basicColors.xml              \
	data/webColors.xml                \
	data/animations/example.xml       \
	data/animations/skeleton.xml      \
	data/profiles/default.xml         \
	data/profiles/skeleton.xml        \
	data/21-ledspicerd.rules          \
	AUTHORS                           \
	ChangeLog                         \
	NEWS
