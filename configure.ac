AC_PREREQ([2.69])
AC_INIT([LEDSpicer],[0.6.3],[https://sourceforge.net/p/ledspicer/tickets],[ledspicer],[https://sourceforge.net/projects/ledspicer])

AC_COPYRIGHT([Copyright © 2018 - 2024 Patricio A. Rossi])
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

LT_INIT([disable-static dlopen])

m4_ifdef([PKG_INSTALLDIR], [PKG_INSTALLDIR], AC_SUBST([pkgconfigdir], ${libdir}/pkgconfig))

AC_SUBST([LIB_VERSION], [1:1:0])
DATA_VERSION=["1.0"]
AC_DEFINE([DATA_VERSION], ["1.0"], [Data files version])
AC_DEFINE([COPYRIGHT], ["Copyright © 2018 - 2024 Patricio A. Rossi (MeduZa)"], [Copyright message])

dnl Enable MiSTer only code.
AC_ARG_ENABLE(mister,
	AS_HELP_STRING([--enable-mister],
	[Enables the compilation of MiSTer only features @<:@default=no@:>@]),
	[case "${enableval}" in
		yes) mister=yes ;;
		no)  mister=no ;;
		*) AC_MSG_ERROR([bad value ${enableval} for --enable-mister]) ;;
	esac],
	[mister=no])
AM_CONDITIONAL([MiSTer],[test x$mister = xyes])

if test x$mister = xno ; then
dnl Enable pulseaudio plugin.
AC_ARG_ENABLE(pulseaudio,
	AS_HELP_STRING([--enable-pulseaudio],
	[Enables the pulseaudio plugin @<:@default=yes@:>@]),
	[case "${enableval}" in
		yes) pulseaudio=yes ;;
		no)  pulseaudio=no ;;
		*) AC_MSG_ERROR([bad value ${enableval} for --enable-pulseaudio]) ;;
	esac],
	[pulseaudio=yes])
else
[pulseaudio=no]
AC_DEFINE([MiSTer],[1],[Enables MiSTer code])
fi
AM_CONDITIONAL([PULSEAUDIO],[test x$pulseaudio = xyes])

dnl Enable alsaaudio plugin.
AC_ARG_ENABLE(alsaaudio,
	AS_HELP_STRING([--enable-alsaaudio],
	[Enables the alsaaudio plugin @<:@default=yes@:>@]),
	[case "${enableval}" in
		yes) alsaaudio=yes ;;
		no)  alsaaudio=no ;;
		*) AC_MSG_ERROR([bad value ${enableval} for --enable-alsaaudio]) ;;
	esac],
	[alsaaudio=yes])

AM_CONDITIONAL([ALSAAUDIO],[test x$alsaaudio = xyes])

dnl ******************
dnl * Output plugins *
dnl ******************

dnl Enable NANOLED plugin.
AC_ARG_ENABLE(nanoled,
	AS_HELP_STRING([--enable-nanoled],
	[Enables the output plugin nanoled @<:@default=no@:>@]),
	[case "${enableval}" in
		yes) nanoled=yes ;;
		no)  nanoled=no ;;
		*) AC_MSG_ERROR([bad value ${enableval} for --enable-nanoled]) ;;
	esac],
	[nanoled=no])
AM_CONDITIONAL([NANOLED],[test x$nanoled = xyes])

dnl Enable PACDRIVE plugin.
AC_ARG_ENABLE(pacdrive,
	AS_HELP_STRING([--enable-pacdrive],
	[Enables the output plugin pacdrive @<:@default=no@:>@]),
	[case "${enableval}" in
		yes) pacdrive=yes ;;
		no)  pacdrive=no ;;
		*) AC_MSG_ERROR([bad value ${enableval} for --enable-pacdrive]) ;;
	esac],
	[pacdrive=no])
AM_CONDITIONAL([PACDRIVE],[test x$pacdrive = xyes])

dnl Enable PACLED64 plugin.
AC_ARG_ENABLE(pacled64,
	AS_HELP_STRING([--enable-pacled64],
	[Enables the output plugin pacled64 @<:@default=no@:>@]),
	[case "${enableval}" in
		yes) pacled64=yes ;;
		no)  pacled64=no ;;
		*) AC_MSG_ERROR([bad value ${enableval} for --enable-pacled64]) ;;
	esac],
	[pacled64=no])
AM_CONDITIONAL([PACLED64],[test x$pacled64 = xyes])

dnl Enable ULTIMATEIO plugin.
AC_ARG_ENABLE(ultimateio,
	AS_HELP_STRING([--enable-ultimateio],
	[Enables the output plugin ultimateio @<:@default=no@:>@]),
	[case "${enableval}" in
		yes) ultimateio=yes ;;
		no)  ultimateio=no ;;
		*) AC_MSG_ERROR([bad value ${enableval} for --enable-ultimateio]) ;;
	esac],
	[ultimateio=no])
AM_CONDITIONAL([ULTIMATEIO],[test x$ultimateio = xyes])

dnl Enable LEDWIZ32 plugin.
AC_ARG_ENABLE(ledwiz32,
	AS_HELP_STRING([--enable-ledwiz32],
	[Enables the output plugin ledwiz32 @<:@default=no@:>@]),
	[case "${enableval}" in
		yes) ledwiz32=yes ;;
		no)  ledwiz32=no ;;
		*) AC_MSG_ERROR([bad value ${enableval} for --enable-ledwiz32]) ;;
	esac],
	[ledwiz32=no])
AM_CONDITIONAL([LEDWIZ32],[test x$ledwiz32 = xyes])

dnl Enable HOWLER plugin.
AC_ARG_ENABLE(howler,
	AS_HELP_STRING([--enable-howler],
	[Enables the output plugin howler @<:@default=no@:>@]),
	[case "${enableval}" in
		yes) howler=yes ;;
		no)  howler=no ;;
		*) AC_MSG_ERROR([bad value ${enableval} for --enable-howler]) ;;
	esac],
	[howler=no])
AM_CONDITIONAL([HOWLER],[test x$howler = xyes])

dnl Enable RASPBERRYPI plugin.
AC_ARG_ENABLE(raspberrypi,
	AS_HELP_STRING([--enable-raspberrypi],
	[Enables the output plugin raspberrypi @<:@default=no@:>@]),
	[case "${enableval}" in
		yes) raspberrypi=yes ;;
		no)  raspberrypi=no ;;
		*) AC_MSG_ERROR([bad value ${enableval} for --enable-raspberrypi]) ;;
	esac],
	[raspberrypi=no])
AM_CONDITIONAL([RASPBERRYPI],[test x$raspberrypi = xyes])

dnl Enable ADALIGHT plugin.
AC_ARG_ENABLE(adalight,
	AS_HELP_STRING([--enable-adalight],
	[Enables the output plugin adalight @<:@default=no@:>@]),
	[case "${enableval}" in
		yes) adalight=yes ;;
		no)  adalight=no ;;
		*) AC_MSG_ERROR([bad value ${enableval} for --enable-adalight]) ;;
	esac],
	[adalight=no])
AM_CONDITIONAL([ADALIGHT],[test x$adalight = xyes])

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
AC_MSG_CHECKING([dependencies])

PKG_CHECK_MODULES([LIBUSB], [libusb-1.0 >= 1.0],
	echo -e "\tlibusb 1.0 ver `pkg-config --short-errors --modversion libusb-1.0` found",
	AC_MSG_ERROR([libusb-1.0 development library not found.])
)

PKG_CHECK_MODULES([TINYXML2], [tinyxml2 >= 6.0],
	echo -e "\ttinyxml2 ver `pkg-config --short-errors --modversion tinyxml2` found",
	AC_MSG_ERROR([tinyxml2 not found.])
)

PKG_CHECK_MODULES([PTHREADSTUBS], pthread-stubs,
	echo -e "\tpthread found"
	[PTHREADSTUBS_LIBS="-lpthread"],
	AC_MSG_ERROR([pthread development library not found.])
)

if test x$pulseaudio = xyes ; then
	AC_MSG_NOTICE([User $USER will be used to allow pulseaudio to work, change it if needed in service file])
	PKG_CHECK_MODULES([LIBPULSE], [libpulse >= 0.9.~],
		echo -e "\tlibpulse ver `pkg-config --short-errors --modversion libpulse` found"
		USERNAME=$USER
		AC_SUBST(USERNAME)
		AC_SUBST([PULSEAUDIO]),
		AC_MSG_ERROR([libpulse development library not found.])
	)
fi

if test x$alsaaudio = xyes ; then
	PKG_CHECK_MODULES([LIBALSA], [alsa >= 0.2],
		echo -e "\talsa ver `pkg-config --short-errors --modversion alsa` found"
		AC_SUBST([ALSAAUDIO]),
		AC_MSG_ERROR([libalsa development library not found.])
	)
fi

if test x$raspberrypi = xyes ; then
	AC_CHECK_HEADER([pigpio.h], [], [AC_MSG_ERROR([pigpio.h header not found.])], [])
	[RASPBERRYPI_LIBS="-lpigpio"]
	AC_SUBST(RASPBERRYPI_LIBS)
fi

dnl expand bindir on configuration files.
AS_AC_EXPAND(BINDIR, $bindir)

dnl *****************
dnl * Needed header *
dnl *****************
AC_MSG_CHECKING([Headers])

AC_CHECK_HEADERS([termios.h syslog.h unistd.h pthread.h fcntl.h netdb.h sys/ioctl.h sys/socket.h ], [], [AC_MSG_ERROR([missing header file.])], [])

dnl ****************
dnl * Needed Types *
dnl ****************
AC_MSG_CHECKING([Types])

AC_CHECK_HEADER_STDBOOL
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_INT8_T
AC_TYPE_UINT8_T
AC_TYPE_INT16_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T

dnl ********************************
dnl * Checks for library functions *
dnl ********************************
AC_FUNC_ERROR_AT_LINE
AC_CHECK_FUNCS([atexit gethostbyname memset select socket strerror])

dnl dlopen
AC_CHECK_LIB([dl], [dlopen],
	[LINKER_LIBS="-ldl"],
	AC_MSG_ERROR([You must have the dl development files installed to build.])
)
AC_SUBST([LINKER_LIBS])

ACTORS_DIR="${libdir}/${PACKAGE_TARNAME}/actors"
AC_SUBST([ACTORS_DIR])
AS_AC_EXPAND(LSAD, $ACTORS_DIR)
DEVICES_DIR="${libdir}/${PACKAGE_TARNAME}/devices"
AC_SUBST([DEVICES_DIR])
AS_AC_EXPAND(LSDD, $DEVICES_DIR)
INPUTS_DIR="${libdir}/${PACKAGE_TARNAME}/inputs"
AC_SUBST([INPUTS_DIR])
AS_AC_EXPAND(LSID, $INPUTS_DIR)
AS_AC_EXPAND(LSDDD, $datadir)

dnl *************
dnl * Makefiles *
dnl *************
AC_CONFIG_FILES([
	Makefile
	data/ledspicer.pc
	data/ledspicerd.service
])

AC_OUTPUT

dnl Dump the build configuration for the developer
AC_MSG_RESULT([--------- build environment -----------
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
Libpulse     : $pulseaudio: $LIBPULSE_CFLAGS $LIBPULSE_LIBS
ALSA         : $alsaaudio: $LIBALSA_CFLAGS $LIBALSA_LIBS
Prefix       : $prefix
Devices dir  : $LSDD
Actors dir   : $LSAD
Inputs dir   : $LSID
Data dir     : $LSDDD
Config dir   : $sysconfdir
Develop mode : $DEVELOP_MODE
MiSTer code  : $mister
--------- Output Plugins -----------
NanoLed      : $nanoled
PacDrive     : $pacdrive
PacLed64     : $pacled64
Ultimate IO  : $ultimateio
LedWiz 32    : $ledwiz32
Howler       : $howler
Raspberry Pi : $raspberrypi
Adalight     : $adalight
])
