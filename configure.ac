AC_PREREQ([2.69])
AC_INIT([LEDSpicer],[0.3.0],[https://sourceforge.net/p/ledspicer/tickets],[ledspicer],[https://sourceforge.net/projects/ledspicer])

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
AC_PROG_LN_S
AC_LIBTOOL_DLOPEN
LT_INIT([disable-static dlopen])

m4_ifdef([PKG_INSTALLDIR], [PKG_INSTALLDIR], AC_SUBST([pkgconfigdir], ${libdir}/pkgconfig))

AC_SUBST([LIB_VERSION], [1:0:0])
DATA_VERSION=["1.0"]
AC_DEFINE([DATA_VERSION], ["1.0"], [Data files version])

dnl Enable copy of extra files: conf and samples.
AC_ARG_ENABLE(samples,
	AS_HELP_STRING([--enable-samples],
	[copy sample conf and data files @<:@default=no@:>@]),
	[case "${enableval}" in
		yes) samples=yes ;;
		no)  samples=no ;;
		*) AC_MSG_ERROR([bad value ${enableval} for --enable-samples]) ;;
	esac],
	[samples=no])
AM_CONDITIONAL([COPY_SAMPLES],[test x$samples = xyes])

dnl Enable develop mode
AC_ARG_ENABLE(develop,
	AS_HELP_STRING([--enable-develop],
	[enable develop mode]),
	AC_DEFINE([DEVELOP],[1],[Enables development mode])
	DEVELOP_MODE=yes,
	DEVELOP_MODE=no
)

dnl ********************
dnl * Needed Libraries *
dnl ********************
AC_MSG_NOTICE([Checking for dependencies])

PKG_CHECK_MODULES([LIBUSB], [libusb-1.0 >= 1.0.21],
	echo -e "\tlibusb 1.0 ver `pkg-config --short-errors --modversion libusb-1.0` found",
	AC_MSG_ERROR([libusb-1.0 development library not found.])
)

PKG_CHECK_MODULES([TINYXML2], [tinyxml2 >= 2.2],
	echo -e "\ttinyxml2 ver `pkg-config --short-errors --modversion tinyxml2` found",
	AC_MSG_ERROR([tinyxml2 not found.])
)

PKG_CHECK_MODULES([PTHREADSTUBS], pthread-stubs,
	echo -e "\tpthread found"
	[PTHREADSTUBS_LIBS="-lpthread"],
	AC_MSG_ERROR([pthread development library not found.])
)

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

dnl dlopen
AC_CHECK_LIB([dl], [dlopen],
	[LINKER_LIBS="-ldl"],
	AC_MSG_ERROR([You must have the dl development files installed to build.])
)
AC_SUBST([LINKER_LIBS])

ACTORS_DIR=["${libdir}/${PACKAGE_TARNAME}/actors"]
AC_SUBST([ACTORS_DIR])
DEVICES_DIR=["${libdir}/${PACKAGE_TARNAME}/devices"]
AC_SUBST([DEVICES_DIR])

dnl *************************
dnl * Daemon initialization *
dnl *************************

AC_ARG_WITH([systemdsystemunitdir],
	AS_HELP_STRING([--with-systemdsystemunitdir=DIR],
	[Directory for systemd service files]),
	[],
	[with_systemdsystemunitdir=$($PKG_CONFIG --variable=systemdsystemunitdir systemd)]
	if test x"$with_systemdsystemunitdir" = x; then
		AC_MSG_NOTICE([Could not detect systemd unit directory])
	fi
)
AC_SUBST([systemdsystemunitdir], [$with_systemdsystemunitdir])

dnl *************
dnl * Makefiles *
dnl *************
AC_CONFIG_FILES([
	Makefile
	data/ledspicerd.service
	data/ledspicer.pc
])

AC_OUTPUT

dnl Dump the build configuration for the developer
echo -e "\n--------- build environment -----------
Program      : $PACKAGE_NAME
Version      : $PACKAGE_VERSION
WebSite      : $PACKAGE_URL
liblespicer  : $LIB_VERSION
DATA Version : $DATA_VERSION
Build system : $build_cpu-$build_vendor-$build_os
--------- build settings -----------
C++ Compiler : $CXX $CPPFLAGS $CXXFLAGS
Linker       : $LD $LDFLAGS $LIBS
Includes     :$LIBUSB_CFLAGS $TINYXML2_CFLAGS $PTHREADSTUBS_CFLAGS $LINKER_CFLAGS
Libraries    :$LIBUSB_LIBS $TINYXML2_LIBS $PTHREADSTUBS_LIBS $LINKER_LIBS
Prefix       : $prefix
Systemd      : $systemdsystemunitdir
Devices dir  : $DEVICES_DIR
Actors dir   : $ACTORS_DIR
Data dir     : $datadir
Config dir   : $sysconfdir
Develop mode : $DEVELOP_MODE
Copy samples : $samples"
