if RTMIDI_STANDALONE
ACLOCAL_AMFLAGS = -I m4 --install
SUBDIRS = po
endif
RTMIDITESTCXXFLAGS = $(EXTRA_RTMIDITESTCXXFLAGS) -I$(top_srcdir)/%D%
RTMIDILIBRARYNAME = %D%/librtmidi@rtmidi_suffix@.la
RTMIDITESTLDFLAGS = $(EXTRA_RTMIDITESTLDFLAGS) $(RTMIDILIBRARYNAME)
RTMIDICONFIGRPATH = config/config.rpath
lib_LTLIBRARIES += %D%/librtmidi@rtmidi_suffix@.la
%C%_librtmidi@rtmidi_suffix@_la_LDFLAGS = -no-undefined $(RTMIDI__TS_LIBS)
%C%_librtmidi@rtmidi_suffix@_la_CXXFLAGS = $(CXXFLAGS) $(CPPFLAGS) $(RTMIDI_TS_API) -DRTMIDI_NO_WARN_DEPRECATED
%C%_librtmidi@rtmidi_suffix@_la_SOURCES = \
  %D%/RtMidi.cpp \
  %D%/RtMidi.h

EXTRA_DIST += \
	$(RTMIDICONFIGRPATH) \
	%D%/m4/ChangeLog \
	%D%/rtmidi-config.in \
	%D%/librtmidi.pc.in

include %D%/tests/Makefile.am
