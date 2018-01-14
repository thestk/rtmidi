# Process this file with autoconf to produce a configure script.
#AC_INIT(RtMidi, 2.1.0, gary@music.mcgill.ca, rtmidi)
AC_INIT(RtMidi-ts, 3.0, keinstein_junoir@gmx.net, rtmidi-ts)
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_SRCDIR(RtMidi.cpp)
AC_CONFIG_FILES([rtmidi-config librtmidi.pc Makefile tests/Makefile po/Makefile.in])
AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects])

AC_CONFIG_MACRO_DIR([m4])

m4_include(configure.library)

AC_OUTPUT

chmod oug+x rtmidi-config
