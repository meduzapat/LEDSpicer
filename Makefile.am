
dist_doc_DATA =	\
	README		\
	COPYING		\
	AUTHORS		\
	ChangeLog	\
	NEWS

EXTRA_DIST = \
	autogen.sh
	
bin_PROGRAMS = \
	ledspicerd

ledspicerd_SOURCES =                 \
	src/utility/Log.cpp              \
	src/utility/Error.cpp            \
	src/utility/Utility.cpp          \
	src/utility/XMLHelper.cpp        \
	src/utility/Color.cpp            \
	src/DataLoader.cpp               \
	src/ConnectionUSB.cpp            \
	src/devices/Element.cpp          \
	src/devices/Group.cpp            \
	src/devices/Device.cpp           \
	src/devices/UltimarcUltimate.cpp \
	src/animations/Actor.cpp         \
	src/animations/Pulse.cpp         \
	src/animations/Serpentine.cpp    \
	src/devices/Profile.cpp          \
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
	
	