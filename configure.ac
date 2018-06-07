AC_PREREQ([2.69])
AC_INIT([LEDSpicer], [0.1.0], [https://sourceforge.net/p/ledspicer/tickets/?source=navbar],[ledspicer],[https://sourceforge.net/projects/ledspicer])
AC_COPYRIGHT([Copyright © 2018 Patricio A. Rossi])
AC_CONFIG_SRCDIR([src/Error.hpp])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CXX

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_TYPE_SIZE_T

# Checks for library functions.

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
