
noinst_PROGRAMS = midiprobe midiout qmidiin cmidiin sysextest

AM_CXXFLAGS = -Wall -I$(top_srcdir)

midiprobe_SOURCES = midiprobe.cpp
midiprobe_LDADD = $(top_builddir)/librtmidi.la

midiout_SOURCES = midiout.cpp
midiout_LDADD = $(top_builddir)/librtmidi.la

qmidiin_SOURCES = qmidiin.cpp
qmidiin_LDADD = $(top_builddir)/librtmidi.la

cmidiin_SOURCES = cmidiin.cpp
cmidiin_LDADD = $(top_builddir)/librtmidi.la

sysextest_SOURCES = sysextest.cpp
sysextest_LDADD = $(top_builddir)/librtmidi.la
