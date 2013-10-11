# Process this file with autoconf to produce a configure script.
AC_INIT(RtMidi, 1.0, gary@music.mcgill.ca, rtmidi)
AC_CONFIG_SRCDIR(RtMidi.cpp)
AC_CONFIG_FILES(tests/Makefile)

# Fill GXX with something before test.
AC_SUBST( GXX, ["no"] )

# Checks for programs.
AC_PROG_CC
AC_PROG_CXX(g++ CC c++ cxx)

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS(sys/ioctl.h unistd.h)

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

# Check for debug
AC_MSG_CHECKING(whether to compile debug version)
AC_ARG_ENABLE(debug,
  [  --enable-debug = enable various debug output],
  [AC_SUBST( debug, [-D__RTMIDI_DEBUG__] ) AC_SUBST( cflags, [-g] ) AC_SUBST( object_path, [Debug] ) AC_MSG_RESULT(yes)],
  [AC_SUBST( debug, [] ) AC_SUBST( cflags, [-O2] ) AC_SUBST( object_path, [Release] ) AC_MSG_RESULT(no)])

# Check compiler and use -Wall if gnu.
if [test $GXX = "yes" ;] then
  AC_SUBST( warn, [-Wall] )
fi

# Checks for package options and external software
AC_CANONICAL_HOST
AC_MSG_CHECKING(for MIDI API)
case $host in
  *-*-linux*)
  AC_SUBST( midi_api, [-D__LINUX_ALSASEQ__] )
  AC_CHECK_LIB(asound, snd_seq_open, , AC_MSG_ERROR(RtMidi in Linux requires the ALSA asound library!))
  # Checks for pthread library.
  AC_CHECK_LIB(pthread, pthread_create, , AC_MSG_ERROR(RtMidi requires the pthread library!))
  ;;

  *-sgi*)
  AC_SUBST( midi_api, ["-D__IRIX_MD__ -LANG:std -w"] )
  AC_MSG_RESULT(using IRIX MD)
  AC_CHECK_LIB(md, mdInit, , AC_MSG_ERROR(IRIX MIDI support requires the md library!) )
  # Checks for pthread library.
  AC_CHECK_LIB(pthread, pthread_create, , AC_MSG_ERROR(RtMidi requires the pthread library!))
  ;;

  *-apple*)
  # Check for CoreAudio framework
  AC_CHECK_HEADER(CoreMIDI/CoreMIDI.h, [AC_SUBST( midi_api, [-D__MACOSX_CORE__] )], [AC_MSG_ERROR(CoreMIDI header files not found!)] )
  AC_SUBST( frameworks, ["-framework CoreMIDI -framework CoreFoundation -framework CoreAudio"] )
  ;;

  *)
  # Default case for unknown realtime systems.
  AC_MSG_ERROR(Unknown system type for MIDI support!)
  ;;
esac

# Checks for library functions.
AC_PROG_GCC_TRADITIONAL

AC_OUTPUT
