SUBDIRS += %D%/po
RTMIDITESTCXXFLAGS += -I$(top_srcdir)/%D%
RTMIDITESTLDFLAGS += -L%D% -lrtmidi-ts
lib_LTLIBRARIES += %D%/librtmidi-ts.la
%C%_librtmidi_ts_la_LDFLAGS = -no-undefined $(RTMIDI__TS_LIBS)
%C%_librtmidi_ts_la_CXXFLAGS = $(CXXFLAGS) $(CPPFLAGS) $(RTMIDI_TS_API)
%C%_librtmidi_ts_la_SOURCES = \
  %D%/RtMidi.cpp \
  %D%/RtMidi.h

EXTRA_DIST += config/config.rpath m4/ChangeLog

include %D%/tests/Makefile.am


