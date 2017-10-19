# Process this file with autoconf to produce a configure script.
AC_INIT(RtMidi, 3.0.0, gary@music.mcgill.ca, rtmidi)
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_SRCDIR(RtMidi.cpp)
AC_CONFIG_FILES([rtmidi-config rtmidi.pc Makefile tests/Makefile doc/Makefile doc/doxygen/Doxyfile])
AM_INIT_AUTOMAKE([1.14 -Wall -Werror foreign subdir-objects])

# libtool version: current:revision:age
#
# If the library source code has changed at all since the last update, then
# increment revision (`c:r:a' becomes `c:r+1:a').
#
# If any interfaces have been added, removed, or changed since the last update,
# increment current, and set revision to 0.
#
# If any interfaces have been added since the last public release, then
# increment age.
#
# If any interfaces have been removed since the last public release, then set
# age to 0.
m4_define([lt_current], 4)
m4_define([lt_revision], 0)
m4_define([lt_age], 0)

m4_define([lt_version_info], [lt_current:lt_revision:lt_age])
m4_define([lt_current_minus_age], [m4_eval(lt_current - lt_age)])

SO_VERSION=lt_version_info
AC_SUBST(SO_VERSION)
AC_SUBST(LIBS)
AC_SUBST(api)
AC_SUBST(req)

api=""
req=""

# Fill GXX with something before test.
GXX="no"

# if the user did not provide any CXXFLAGS, we can override them
AS_IF([test "x$CXXFLAGS" = "x" ], [override_cxx=yes], [override_cxx=no])
AS_IF([test "x$CFLAGS" = "x" ], [override_c=yes], [override_c=no])

# Check version number coherency between RtMidi.h and configure.ac
AC_MSG_CHECKING([that version numbers are coherent])
RTMIDI_VERSION=`sed -n 's/#define RTMIDI_VERSION "\(.*\)"/\1/p' $srcdir/RtMidi.h`
AS_IF(
   [test "x$RTMIDI_VERSION" != "x$PACKAGE_VERSION"],
   [AC_MSG_FAILURE([testing RTMIDI_VERSION==PACKAGE_VERSION failed, check that RtMidi.h defines RTMIDI_VERSION as "$PACKAGE_VERSION" or that the first line of configure.ac has been updated.])])

# Enable some nice automake features if they are available
m4_ifdef([AM_MAINTAINER_MODE], [AM_MAINTAINER_MODE])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

# configure flags
AC_ARG_ENABLE([debug], [AS_HELP_STRING([--enable-debug], [enable various debugging output])])
AC_ARG_WITH(jack, [AS_HELP_STRING([--with-jack], [choose JACK server support (mac and linux only)])])
AC_ARG_WITH(alsa, [AS_HELP_STRING([--with-alsa], [choose native ALSA sequencer API support (linux only)])])
AC_ARG_WITH(core, [AS_HELP_STRING([--with-core], [ choose CoreMidi API support (mac only)])])
AC_ARG_WITH(winmm, [AS_HELP_STRING([--with-winmm], [ choose Windows MultiMedia (MM) API support (win32 only)])])
AC_ARG_WITH(winks, [AS_HELP_STRING([--with-winks], [  choose kernel streaming support (win32 only)])])


# Checks for programs.
AC_PROG_CXX(g++ CC c++ cxx)
AM_PROG_AR
AC_PATH_PROG(AR, ar, no)
AS_IF([test "x$AR" = "xno"], [
    AC_MSG_ERROR([Could not find ar - needed to create a library])
])

LT_INIT([win32-dll])
AC_CONFIG_MACRO_DIR([m4])

# Checks for header files.
AC_HEADER_STDC
#AC_CHECK_HEADERS(sys/ioctl.h unistd.h)

# Check for POSIX semaphore support
AC_CHECK_HEADER([semaphore.h], [
  AC_CHECK_LIB(pthread, sem_init,
    AC_DEFINE([HAVE_SEMAPHORE],[1],[Define to 1 if you have POSIX semaphore support on your system.]),
    AC_MSG_WARN([POSIX semaphore support not found; data may be lost on closePort]))
])


# check for debug
AC_MSG_CHECKING(whether to compile debug version)
debugflags=""
object_path=Release
AS_CASE([${enable_debug}],
  [ yes ], [
    AC_MSG_RESULT([yes])
    CPPFLAGS="-D__RTMIDI_DEBUG ${CPPFLAGS}"
    debugflags="${debugflags} -g"
    object_path=Debug
  ],
  [ no ], [
    AC_MSG_RESULT([no!])
    debugflags="${debugflags} -O3"
  ], [
    AC_MSG_RESULT([no])
  ])
# For debugging and optimization ... overwrite default because it has both -g and -O2
AS_IF([test "x$debugflags" != x],
  AS_IF([test "x$override_cxx" = "xyes" ], CXXFLAGS="$CXXFLAGS $debugflags", CXXFLAGS="$debugflags $CXXFLAGS")
  AS_IF([test "x$override_c" = "xyes" ], CFLAGS="$CFLAGS $debugflags", CFLAGS="$debugflags $CFLAGS")
  )

# Check compiler and use -Wall if gnu.
AS_IF([test "x$GXX" = "xyes"], [
  CXXFLAGS="-Wall -Wextra ${CXXFLAGS}"
])

# Checks for doxygen
AC_CHECK_PROG( DOXYGEN, [doxygen], [doxygen] )
AM_CONDITIONAL( MAKE_DOC, [test "x${DOXYGEN}" != x] )

# Copy doc files to build dir if necessary
AC_CONFIG_LINKS( [doc/doxygen/footer.html:doc/doxygen/footer.html] )
AC_CONFIG_LINKS( [doc/doxygen/header.html:doc/doxygen/header.html] )
AC_CONFIG_LINKS( [doc/doxygen/tutorial.txt:doc/doxygen/tutorial.txt] )
AC_CONFIG_LINKS( [doc/images/ccrma.gif:doc/images/ccrma.gif] )
AC_CONFIG_LINKS( [doc/images/mcgill.gif:doc/images/mcgill.gif] )

# Checks for package options and external software
AC_CANONICAL_HOST




AC_MSG_CHECKING(for MIDI API)

AS_IF([test "x$with_jack" = "xyes"], [
api="$api -D__UNIX_JACK__"
AC_MSG_RESULT(using JACK)
AC_CHECK_LIB(jack, jack_client_open, , AC_MSG_ERROR(JACK support requires the jack library!))])

case $host in
  *-*-linux*)
  # Look for ALSA flag
  AS_IF([test "x$with_alsa" = "xyes"], [
    AC_MSG_RESULT(using ALSA)
    AC_CHECK_LIB(asound, snd_seq_open, , AC_MSG_ERROR(ALSA support requires the asound library!))
    api="$api -D__LINUX_ALSA__"
    req="$req alsa"
  ])

  AS_IF([test "x${api}" = "x"],
    AC_MSG_RESULT(using ALSA)
    AC_CHECK_LIB(asound, snd_seq_open, , AC_MSG_ERROR(ALSA sequencer support requires the asound library!))
    api="-D__LINUX_ALSA__ ${api}"
    req="$req alsa"
  )

  # Checks for pthread library.
  AC_CHECK_LIB(pthread, pthread_create, , AC_MSG_ERROR(RtMidi requires the pthread library!))
  ;;

  *-apple*)
  # Look for Core flag
  AS_IF([test "x$with_core" = "xyes"], [
    AC_MSG_RESULT(using CoreMidi)
    AC_CHECK_HEADER(CoreMIDI/CoreMIDI.h, [], [AC_MSG_ERROR(CoreMIDI header files not found!)] )
    LIBS="-framework CoreMIDI -framework CoreFoundation -framework CoreAudio ${LIBS}"
    api="$api -D__MACOSX_CORE__"
  ])

  # If no api flags specified, use CoreMidi
  AS_IF([test "x${api}" = "x"],
    AC_MSG_RESULT(using CoreMidi)
    AC_CHECK_HEADER(CoreMIDI/CoreMIDI.h,
      [],
      [AC_MSG_ERROR(CoreMIDI header files not found!)] )
    LIBS="-framework CoreMIDI -framework CoreFoundation -framework CoreAudio ${LIBS}"
    api="$api -D__MACOSX_CORE__"
  )
  ;;

  *-mingw32*)
  # Look for WinMM flag
  AS_IF([test "x$with_winmm" = "xyes"], [
    AC_MSG_RESULT(using WinMM)
    LIBS="-lwinmm ${LIBS}"
    api="$api -D__WINDOWS_MM__"
  ])

  AS_IF([test "x$with_winks" = "xyes"], [
    AC_MSG_RESULT(using kernel streaming)
    LIBS="-lsetupapi -lksuser ${LIBS}"
    api="$api -D__WINDOWS_KS__"
  ])

  # I can't get the following check to work so just manually add the library
	# or could try the following?  AC_LIB_WINMM([midiOutGetNumDevs])
  # AC_CHECK_LIB(winmm, midiInGetNumDevs, , AC_MSG_ERROR(Windows MIDI support requires the winmm library!) )],)

  # If no api flags specified, use WinMM
  AS_IF([test "x${api}" = "x"], [
    AC_MSG_RESULT(using WinMM)
    LIBS="-lwinmm ${LIBS}"
    api="${api} -D__WINDOWS_MM__"
  ])
  ;;

  *)
  # Default case for unknown realtime systems.
  AC_MSG_ERROR(Unknown system type for MIDI support!)
  ;;
esac

CPPFLAGS="$CPPFLAGS $api"

AC_OUTPUT

chmod oug+x rtmidi-config
