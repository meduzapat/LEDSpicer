
bin_PROGRAMS = \
	ledspicerd

ledspicerd_SOURCES = \
	Error.hpp        \
	Error.cpp        \
	Main.hpp         \
	Main.cpp

ledspicerd_CPPFLAGS =                     \
	-DPACKAGE_CONF_DIR=\"$(sysconfdir)\"  \
	-DPACKAGE_LOCALE_DIR=\"$(localedir)\" \
	-DPACKAGE_SRC_DIR=\"$(srcdir)\"       \
	-DPACKAGE_DATA_DIR=\"$(pkgdatadir)\"  \
	-DPACKAGE_LIB_DIR=\"$(pkglibdir)\"

ledspicerd_CXXFLAGS =    \
	$(LEDSPICERD_CFLAGS) \
	-Wl,--no-as-needed
