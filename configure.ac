AC_PREREQ([2.69])
AC_INIT([LEDSpicer], [0.1.0], [https://sourceforge.net/p/ledspicer/tickets/?source=navbar],[ledspicer],[https://sourceforge.net/projects/ledspicer])
AC_COPYRIGHT([Copyright © 2018 Patricio A. Rossi])
AC_CONFIG_SRCDIR([src/Error.hpp])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE
AM_SILENT_RULES
AC_CONFIG_MACRO_DIR([m4])

dnl *******************
dnl * Needed Programs *
dnl *******************
AC_MSG_CHECKING([Programs])

AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL

dnl ********************
dnl * Needed Libraries *
dnl ********************

dnl *****************
dnl * Needed header *
dnl *****************
AC_MSG_CHECKING([Headers])

AC_CHECK_HEADERS([unistd.h])

dnl ****************
dnl * Needed Types *
dnl ****************
AC_MSG_CHECKING([Types])

AC_CHECK_HEADER_STDBOOL
AC_TYPE_SIZE_T

dnl *************
dnl * Makefiles *
dnl *************
AC_CONFIG_FILES([
	Makefile
	src/Makefile
])

AC_OUTPUT

dnl Dump the build configuration for the developer
echo -e "\n--------- build environment -----------
$PACKAGE_NAME: $PACKAGE_VERSION
WebSite      : $PACKAGE_URL

Build system : $build_cpu-$build_vendor-$build_os
C++ compiler : $CXX
Prefix       : $prefix
Configuration: $sysconfdir
CPPFLAGS     : ${CPPFLAGS}
CXXFLAGS     : ${CXXFLAGS}
Libraries    : ${LEDSPICER_LIBS}
Linker       : $LD

Plugins
Output       : ${OUTPUT_PLUGINS}
Input        : ${INPUT_PLUGINS}"
