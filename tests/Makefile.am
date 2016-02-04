
noinst_PROGRAMS = midiprobe midiout qmidiin cmidiin sysextest

midiprobe_CXXFLAGS = -Wall -I$(top_srcdir)
midiprobe_SOURCES = midiprobe.cpp
midiprobe_LDADD = $(top_builddir)/librtmidi.la

midiout_CXXFLAGS = -Wall -I$(top_srcdir)
midiout_SOURCES = midiout.cpp
midiout_LDADD = $(top_builddir)/librtmidi.la

qmidiin_CXXFLAGS = -Wall -I$(top_srcdir)
qmidiin_SOURCES = qmidiin.cpp
qmidiin_LDADD = $(top_builddir)/librtmidi.la

cmidiin_CXXFLAGS = -Wall -I$(top_srcdir)
cmidiin_SOURCES = cmidiin.cpp
cmidiin_LDADD = $(top_builddir)/librtmidi.la

sysextest_CXXFLAGS = -Wall -I$(top_srcdir)
sysextest_SOURCES = sysextest.cpp
sysextest_LDADD = $(top_builddir)/librtmidi.la
