# Process this file with autoconf to produce a configure script.
AC_INIT(RtMidi, 1.0, gary@music.mcgill.ca, rtmidi)
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_SRCDIR(RtMidi.cpp)
AC_CONFIG_FILES(tests/Makefile)

# Fill GXX with something before test.
AC_SUBST( GXX, ["no"] )

# Checks for programs.
AC_PROG_CXX(g++ CC c++ cxx)

# Checks for header files.
AC_HEADER_STDC
#AC_CHECK_HEADERS(sys/ioctl.h unistd.h)

# Check for debug
AC_MSG_CHECKING(whether to compile debug version)
AC_ARG_ENABLE(debug,
  [  --enable-debug = enable various debug output],
  [AC_SUBST( cppflag, [-D__RTMIDI_DEBUG__] ) AC_SUBST( cxxflag, [-g] ) AC_SUBST( object_path, [Debug] ) AC_MSG_RESULT(yes)],
  [AC_SUBST( cppflag, [] ) AC_SUBST( cxxflag, [-O3] ) AC_SUBST( object_path, [Release] ) AC_MSG_RESULT(no)])

# For -I and -D flags
CPPFLAGS="$CPPFLAGS $cppflag"

# For debugging and optimization ... overwrite default because it has both -g and -O2
#CXXFLAGS="$CXXFLAGS $cxxflag"
CXXFLAGS="$cxxflag"

# Check compiler and use -Wall if gnu.
if [test $GXX = "yes" ;] then
  AC_SUBST( cxxflag, [-Wall] )
fi

CXXFLAGS="$CXXFLAGS $cxxflag"

# Checks for package options and external software
AC_CANONICAL_HOST
AC_MSG_CHECKING(for MIDI API)
case $host in
  *-*-linux*)
  AC_SUBST( api, [""] )
  AC_ARG_WITH(jack, [  --with-jack = choose JACK server support (linux only)], [
  AC_SUBST( api, [-D__LINUX_JACK__] )
  AC_MSG_RESULT(using JACK)
  AC_CHECK_LIB(jack, jack_client_open, , AC_MSG_ERROR(JACK support requires the jack library!))], )

  if [test "$api" == "";] then
    AC_SUBST( api, [-D__LINUX_ALSASEQ__] )
    AC_CHECK_LIB(asound, snd_seq_open, , AC_MSG_ERROR(RtMidi in Linux requires the ALSA asound library!))
    # Checks for pthread library.
    AC_CHECK_LIB(pthread, pthread_create, , AC_MSG_ERROR(RtMidi requires the pthread library!))
  fi

  ;;

  *-sgi*)
  AC_SUBST( api, ["-D__IRIX_MD__ -LANG:std -w"] )
  AC_MSG_RESULT(using IRIX MD)
  AC_CHECK_LIB(md, mdInit, , AC_MSG_ERROR(IRIX MIDI support requires the md library!) )
  # Checks for pthread library.
  AC_CHECK_LIB(pthread, pthread_create, , AC_MSG_ERROR(RtMidi requires the pthread library!))
  ;;

  *-apple*)
  # Check for CoreAudio framework
  AC_CHECK_HEADER(CoreMIDI/CoreMIDI.h, [AC_SUBST( api, [-D__MACOSX_CORE__] )], [AC_MSG_ERROR(CoreMIDI header files not found!)] )
  AC_SUBST( LIBS, ["-framework CoreMIDI -framework CoreFoundation -framework CoreAudio"] )
  ;;

  *-mingw32*)
  # Use WinMM library
  AC_SUBST( api, [-D__WINDOWS_MM__] )
  # I can't get the following check to work so just manually add the library
  # AC_CHECK_LIB(winmm, midiInGetNumDevs, , AC_MSG_ERROR(Windows MIDI support requires the winmm library!) )
  AC_SUBST( LIBS, [-lwinmm] )
  ;;

  *)
  # Default case for unknown realtime systems.
  AC_MSG_ERROR(Unknown system type for MIDI support!)
  ;;
esac

CPPFLAGS="$CPPFLAGS $api"

AC_OUTPUT
