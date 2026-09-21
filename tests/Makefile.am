
noinst_PROGRAMS = midiprobe midiout qmidiin cmidiin sysextest midiclock_in midiclock_out	\
	apinames testcapi

AM_CXXFLAGS = -Wall -I$(top_srcdir)
AM_CFLAGS = -Wall -I$(top_srcdir)

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

midiclock_in_SOURCES = midiclock.cpp
midiclock_in_LDADD = $(top_builddir)/librtmidi.la

midiclock_out_SOURCES = midiclock.cpp
midiclock_out_LDADD = $(top_builddir)/librtmidi.la

apinames_SOURCES = apinames.cpp
apinames_LDADD = $(top_builddir)/librtmidi.la

testcapi_SOURCES = testcapi.c
testcapi_LDADD = $(top_builddir)/librtmidi.la
# testcapi.c is C, but it links against a C++ library.  With a shared
# librtmidi the C++ runtime is pulled in through the library's own
# dependencies; a static build has no such link, so the C linker fails on
# every C++ symbol it needs.  Naming a C++ source here makes Automake choose
# the C++ linker in both cases.  The file is never compiled and need not exist.
nodist_EXTRA_testcapi_SOURCES = dummy.cxx

EXTRA_DIST = cmidiin.dsp midiout.dsp midiprobe.dsp qmidiin.dsp	\
	sysextest.dsp RtMidi.dsw AlsaHotplug.cmake RunAlsaHotplug.cmake README-hotplug.md

TESTS = apinames

if HAVE_ALSA
noinst_PROGRAMS += alsahotplug
alsahotplug_SOURCES = alsahotplug.cpp $(top_srcdir)/RtMidi.cpp
alsahotplug_CPPFLAGS = $(AM_CPPFLAGS) $(CPPFLAGS) -U__UNIX_JACK__ -D__RTMIDI_DUMMY__
alsahotplug_LDFLAGS = -Wl,--wrap=snd_seq_client_info_get_card -Wl,--wrap=realpath \
  -Wl,--wrap=__realpath_chk -Wl,--wrap=pipe2 -Wl,--wrap=pthread_create \
  -Wl,--wrap=snd_seq_open -Wl,--wrap=snd_seq_event_input -Wl,--wrap=snd_seq_subscribe_port \
  -Wl,--wrap=snd_seq_poll_descriptors -Wl,--wrap=snd_seq_query_next_client
TESTS += alsahotplug
if NATIVE_BUILD
# Match CMake: building ALSA also runs its noninteractive regression test.
all-local: alsahotplug
	$(MAKE) $(AM_MAKEFLAGS) check-TESTS TESTS=alsahotplug
endif
endif
