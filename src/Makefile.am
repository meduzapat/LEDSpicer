
bin_PROGRAMS = \
	ledspicerd

ledspicerd_SOURCES = \
	Log.hpp          \
	Log.cpp          \
	Error.hpp        \
	Error.cpp        \
	XMLHelper.hpp    \
	XMLHelper.cpp    \
	ConnectionUSB.hpp\
	ConnectionUSB.cpp\
	Device.hpp       \
	Device.cpp       \
	Main.hpp         \
	Main.cpp

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