AC_PREREQ([2.69])
AC_INIT([LEDSpicer],[0.2.1],[https://sourceforge.net/p/ledspicer/tickets],[ledspicer],[https://sourceforge.net/projects/ledspicer])

AC_COPYRIGHT([Copyright © 2018 Patricio A. Rossi])
AC_CONFIG_SRCDIR([src/utility/Error.cpp])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
AC_CANONICAL_HOST

AM_INIT_AUTOMAKE([subdir-objects])
AM_SILENT_RULES

dnl *******************
dnl * Needed Programs *
dnl *******************
AC_MSG_CHECKING([Programs])

AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_AWK
AC_PROG_MKDIR_P

AC_ARG_ENABLE(develop, AS_HELP_STRING([--enable-develop], [enable develop mode]),
	AC_DEFINE([DEVELOP],[1],[Enables development mode])
	DEVELOP_MODE=1
	CXXFLAGS='-g3 -O0',
	DEVELOP_MODE=0
)

dnl ********************
dnl * Needed Libraries *
dnl ********************
AC_MSG_NOTICE([Checking for dependencies])

LIBUSB_VER=1.0.21
LIBTINYXML_VER=2.2

PKG_CHECK_MODULES([LIBUSB],[libusb-1.0 >= $LIBUSB_VER],
	echo -e "\tlibusb 1.0 ver `pkg-config --short-errors --modversion libusb-1.0` found",
	AC_MSG_ERROR([libusb-1.0 development library not found.])
)

PKG_CHECK_MODULES([TINYXML2],[tinyxml2 >= LIBTINYXML_VER],
	echo -e "\ttinyxml2 ver `pkg-config --short-errors --modversion tinyxml2` found",
	AC_MSG_ERROR([tinyxml2 not found.])
)

PKG_CHECK_MODULES(PTHREADSTUBS,pthread-stubs,
	echo -e "\tpthread found"
	[LEDSPICER_LIBS+=" -lpthread"],
	AC_MSG_ERROR([pthread development library not found.])
)

LEDSPICER_LIBS+=" $LIBUSB_LIBS $TINYXML2_LIBS $PTHREADSTUBS_LIBS"
AC_SUBST(LEDSPICER_LIBS)

LEDSPICER_CPPFLAGS+=" $LIBUSB_CFLAGS $TINYXML2_CFLAGS $PTHREADSTUBS_CFLAGS"
AC_SUBST(LEDSPICER_CPPFLAGS)

dnl expand bindir on configuration files.
AS_AC_EXPAND(BINDIR, $bindir)

dnl *****************
dnl * Needed header *
dnl *****************
AC_MSG_CHECKING([Headers])

AC_CHECK_HEADERS([syslog.h unistd.h pthread.h fcntl.h netdb.h sys/ioctl.h sys/socket.h ])

dnl ****************
dnl * Needed Types *
dnl ****************
AC_MSG_CHECKING([Types])

AC_CHECK_HEADER_STDBOOL
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_INT8_T
AC_TYPE_UINT8_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T

dnl ********************************
dnl * Checks for library functions *
dnl ********************************
AC_FUNC_ERROR_AT_LINE
AC_CHECK_FUNCS([gethostbyname memset select socket strerror])

dnl *************************
dnl * Daemon initialization *
dnl *************************

AC_ARG_WITH([systemdsystemunitdir],
	AS_HELP_STRING([--with-systemdsystemunitdir=DIR],
	[Directory for systemd service files]),
	[],
	[with_systemdsystemunitdir=$($PKG_CONFIG --variable=systemdsystemunitdir systemd)])
AC_SUBST([systemdsystemunitdir], [$with_systemdsystemunitdir])

dnl *************
dnl * Makefiles *
dnl *************
AC_CONFIG_FILES([
	Makefile
	data/ledspicerd.service
])

AC_OUTPUT

dnl Dump the build configuration for the developer
echo -e "\n--------- build environment -----------
Program      : ${PACKAGE_NAME}
Version      : ${PACKAGE_VERSION}
WebSite      : ${PACKAGE_URL}
Build system : ${build_cpu}-${build_vendor}-${build_os}
SystemD      : ${systemdsystemunitdir}
--------- build settings -----------
C++ compiler : ${CXX}
Preprocessor :${LEDSPICER_CPPFLAGS}
Libraries    :${LEDSPICER_LIBS}
C++ Flags    : ${CXXFLAGS}
Prefix       : ${prefix}"
if test "${DEVELOP_MODE}" = "1"; then
    echo "Develop mode : On"
else
    echo "Develop mode : Off"
fi