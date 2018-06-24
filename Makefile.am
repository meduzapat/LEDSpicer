
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
	src/utility/Log.hpp              \
	src/utility/Log.cpp              \
	src/utility/Error.hpp            \
	src/utility/Error.cpp            \
	src/utility/XMLHelper.hpp        \
	src/utility/XMLHelper.cpp        \
	src/utility/Color.hpp            \
	src/utility/Color.cpp            \
	src/DataLoader.hpp               \
	src/DataLoader.cpp               \
	src/ConnectionUSB.hpp            \
	src/ConnectionUSB.cpp            \
	src/devices/Element.hpp          \
	src/devices/Element.cpp          \
	src/devices/Group.hpp            \
	src/devices/Group.cpp            \
	src/devices/Device.hpp           \
	src/devices/Device.cpp           \
	src/devices/UltimarcUltimate.hpp \
	src/devices/UltimarcUltimate.cpp \
	src/Main.hpp                     \
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
	
	