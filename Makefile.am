RTMIDITESTCXXFLAGS = $(EXTRA_RTMIDITESTCXXFLAGS) -I$(top_srcdir)/%D%
RTMIDILIBRARYNAME = %D%/librtmidi@rtmidi_suffix@.la
RTMIDITESTLDFLAGS = $(EXTRA_RTMIDITESTLDFLAGS) $(RTMIDILIBRARYNAME)
lib_LTLIBRARIES += %D%/librtmidi@rtmidi_suffix@.la
%C%_librtmidi@rtmidi_suffix@_la_LDFLAGS = -no-undefined $(RTMIDI__TS_LIBS)
%C%_librtmidi@rtmidi_suffix@_la_CXXFLAGS = $(CXXFLAGS) $(CPPFLAGS) $(RTMIDI_TS_API) -DRTMIDI_NO_WARN_DEPRECATED
%C%_librtmidi@rtmidi_suffix@_la_SOURCES = \
  %D%/RtMidi.cpp \
  %D%/RtMidi.h

EXTRA_DIST += config/config.rpath m4/ChangeLog

include %D%/tests/Makefile.am
