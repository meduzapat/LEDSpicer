
dist_doc_DATA =	\
	README		\
	COPYING		\
	AUTHORS		\
	ChangeLog	\
	NEWS

EXTRA_DIST = \
	autogen.sh
	
bin_PROGRAMS = \
	ledspicerd \
	emitter

ledspicerd_SOURCES =                 \
	src/utility/Log.cpp              \
	src/utility/Error.cpp            \
	src/utility/Utility.cpp          \
	src/utility/Socks.cpp            \
	src/utility/XMLHelper.cpp        \
	src/utility/Color.cpp            \
	src/DataLoader.cpp               \
	src/ConnectionUSB.cpp            \
	src/devices/Element.cpp          \
	src/devices/Group.cpp            \
	src/devices/Device.cpp           \
	src/devices/UltimarcUltimate.cpp \
	src/animations/Actor.cpp         \
	src/animations/Gradient.cpp      \
	src/animations/Pulse.cpp         \
	src/animations/Serpentine.cpp    \
	src/animations/Random.cpp        \
	src/animations/Fill.cpp          \
	src/devices/Profile.cpp          \
	src/Messages.cpp                 \
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
	
emitter_SOURCES =                    \
	src/utility/Log.cpp              \
	src/utility/Error.cpp            \
	src/utility/Utility.cpp          \
	src/utility/Socks.cpp            \
	src/Emitter.cpp

emitter_CXXFLAGS =    \
	${CXXFLAGS}       \
	-Wl,--no-as-needed

emitter_LDFLAGS =    \
	${LEDSPICER_LIBS}
	