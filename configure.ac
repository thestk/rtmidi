# Process this file with autoconf to produce a configure script.
#AC_INIT(RtMidi, 2.1.0, gary@music.mcgill.ca, rtmidi)

m4_include(configure.version)

m4_define([ac_rtmidi_version],[lt_current.lt_revision.lt_age])


AC_INIT(RtMidi-ts, ac_rtmidi_version, keinstein_junior@gmx.net, rtmidi-ts)
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_SRCDIR(RtMidi.cpp)
AC_CONFIG_FILES([Makefile doc/Makefile po/Makefile.in rtmidi-config rtmidi.pc  doc/doxygen/Doxyfile],[chmod oug+x rtmidi-config])
AM_INIT_AUTOMAKE([1.14 -Wall -Werror foreign subdir-objects])




# Enable some nice automake features if they are available
m4_ifdef([AM_MAINTAINER_MODE], [AM_MAINTAINER_MODE])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

# Fill GXX with something before test.
AC_SUBST( GXX, ["no"] )

AC_CONFIG_MACRO_DIR([m4])

# Set paths if prefix is defined
if test "x$prefix" != "x" && test "x$prefix" != "xNONE"; then
  LIBS="$LIBS -L$prefix/lib"
  CPPFLAGS="$CPPFLAGS -I$prefix/include/rtmidi"
fi

rtmidi_suffix="-ts"
rtmidi_standalone="yes"
SUBDIRS="$SUBDIRS %D%/po"
m4_include(configure.library)


AC_OUTPUT

