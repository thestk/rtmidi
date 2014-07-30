### RtMidi tests Makefile - for various flavors of unix
AM_CPPFLAGS = -I$(top_srcdir)
LDADD = -L$(top_builddir) -lrtmidi @LIBS@

noinst_PROGRAMS = \
	midiprobe \
	midiout \
	qmidiin \
	cmidiin \
	sysextest \
	midiprobe2 \
	midiprobe-all \
	cmidiin2 \
	qmidiin2 \
	midiout2 \
	loopback \
	errors

TESTS = \
	midiprobe \
	midiprobe2 \
	midiprobe-all \
	errors

if HAVE_VIRTUAL_DEVICES
TESTS += loopback
endif



midiprobe_SOURCES     = midiprobe.cpp
midiout_SOURCES       = midiout.cpp
qmidiin_SOURCES       = qmidiin.cpp
cmidiin_SOURCES       = cmidiin.cpp
sysextest_SOURCES     = sysextest.cpp
midiprobe2_SOURCES    = midiprobe2.cpp
midiprobe_all_SOURCES = midiprobe-all.cpp
cmidiin2_SOURCES      = cmidiin2.cpp
qmidiin2_SOURCES      = qmidiin2.cpp
midiout2_SOURCES      = midiout2.cpp
loopback_SOURCES      = loopback.cpp
errors_SOURCES        = errors.cpp
