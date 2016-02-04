lib_LTLIBRARIES = %D%/librtmidi.la
%C%_librtmidi_la_LDFLAGS = -no-undefined
%C%_librtmidi_la_SOURCES = \
  %D%/RtMidi.cpp \
  %D%/rtmidi_c.cpp

rtmidi_incdir = $(includedir)/rtmidi
rtmidi_inc_HEADERS = \
  %D%/RtMidi.h \
  %D%/rtmidi_c.h

pkgconfigdatadir = $(libdir)/pkgconfig
pkgconfigdata_DATA = rtmidi.pc
