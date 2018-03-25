### RtMidi tests Makefile - for various flavors of unix
#RTMIDITESTCXXFLAGS += -I$(top_srcdir)/%D%
#RTMIDITESTLDFLAGS  += -L$(top_builddir)/%D% -lrtmidi@rtmidi_suffix@ @LIBS@

check_PROGRAMS += \
	%D%/midiprobe \
	%D%/midiout \
	%D%/qmidiin \
	%D%/cmidiin \
	%D%/sysextest \
	%D%/midiprobe2 \
	%D%/midiprobe-all \
	%D%/cmidiin2 \
	%D%/qmidiin2 \
	%D%/midiout2 \
	%D%/loopback \
	%D%/errors

TESTS += \
	%D%/midiprobe \
	%D%/midiprobe2 \
	%D%/midiprobe-all \
	%D%/errors


if RTMIDI_HAVE_VIRTUAL_DEVICES
TESTS += %D%/loopback
endif


%C%_midiprobe_SOURCES      = %D%/midiprobe.cpp
%C%_midiout_SOURCES        = %D%/midiout.cpp
%C%_qmidiin_SOURCES        = %D%/qmidiin.cpp
%C%_cmidiin_SOURCES        = %D%/cmidiin.cpp
%C%_sysextest_SOURCES      = %D%/sysextest.cpp
%C%_midiprobe2_SOURCES     = %D%/midiprobe2.cpp
%C%_midiprobe_all_SOURCES  = %D%/midiprobe-all.cpp
%C%_cmidiin2_SOURCES       = %D%/cmidiin2.cpp
%C%_qmidiin2_SOURCES       = %D%/qmidiin2.cpp
%C%_midiout2_SOURCES       = %D%/midiout2.cpp
%C%_loopback_SOURCES       = %D%/loopback.cpp
%C%_errors_SOURCES         = %D%/errors.cpp

# When a nonstandard gettext library or wrapper is used,
# we need extra flags.
%C%_midiprobe_CXXFLAGS     = $(AM_CXXFLAGS) $(RTMIDITESTCXXFLAGS) -DRTMIDI_NO_WARN_DEPRECATED
%C%_midiout_CXXFLAGS       = $(AM_CXXFLAGS) $(RTMIDITESTCXXFLAGS) -DRTMIDI_NO_WARN_DEPRECATED
%C%_qmidiin_CXXFLAGS       = $(AM_CXXFLAGS) $(RTMIDITESTCXXFLAGS) -DRTMIDI_NO_WARN_DEPRECATED
%C%_cmidiin_CXXFLAGS       = $(AM_CXXFLAGS) $(RTMIDITESTCXXFLAGS) -DRTMIDI_NO_WARN_DEPRECATED
%C%_sysextest_CXXFLAGS     = $(AM_CXXFLAGS) $(RTMIDITESTCXXFLAGS) -DRTMIDI_NO_WARN_DEPRECATED
%C%_midiprobe2_CXXFLAGS    = $(AM_CXXFLAGS) $(RTMIDITESTCXXFLAGS) -DRTMIDI_NO_WARN_DEPRECATED
%C%_midiprobe_all_CXXFLAGS = $(AM_CXXFLAGS) $(RTMIDITESTCXXFLAGS) -DRTMIDI_NO_WARN_DEPRECATED
%C%_cmidiin2_CXXFLAGS      = $(AM_CXXFLAGS) $(RTMIDITESTCXXFLAGS) -DRTMIDI_NO_WARN_DEPRECATED
%C%_qmidiin2_CXXFLAGS      = $(AM_CXXFLAGS) $(RTMIDITESTCXXFLAGS) -DRTMIDI_NO_WARN_DEPRECATED
%C%_midiout2_CXXFLAGS      = $(AM_CXXFLAGS) $(RTMIDITESTCXXFLAGS) -DRTMIDI_NO_WARN_DEPRECATED
%C%_loopback_CXXFLAGS      = $(AM_CXXFLAGS) $(RTMIDITESTCXXFLAGS) -DRTMIDI_NO_WARN_DEPRECATED
%C%_errors_CXXFLAGS        = $(AM_CXXFLAGS) $(RTMIDITESTCXXFLAGS) -DRTMIDI_NO_WARN_DEPRECATED

#
#%C%_midiprobe_LDFLAGS      = $(AM_LDFLAGS) $(RTMIDITESTLDFLAGS)
#%C%_midiout_LDFLAGS        = $(AM_LDFLAGS) $(RTMIDITESTLDFLAGS)
#%C%_qmidiin_LDFLAGS        = $(AM_LDFLAGS) $(RTMIDITESTLDFLAGS)
#%C%_cmidiin_LDFLAGS        = $(AM_LDFLAGS) $(RTMIDITESTLDFLAGS)
#%C%_sysextest_LDFLAGS      = $(AM_LDFLAGS) $(RTMIDITESTLDFLAGS)
#%C%_midiprobe2_LDFLAGS     = $(AM_LDFLAGS) $(RTMIDITESTLDFLAGS)
#%C%_midiprobe_all_LDFLAGS  = $(AM_LDFLAGS) $(RTMIDITESTLDFLAGS)
#%C%_cmidiin2_LDFLAGS       = $(AM_LDFLAGS) $(RTMIDITESTLDFLAGS)
#%C%_qmidiin2_LDFLAGS       = $(AM_LDFLAGS) $(RTMIDITESTLDFLAGS)
#%C%_midiout2_LDFLAGS       = $(AM_LDFLAGS) $(RTMIDITESTLDFLAGS)
#%C%_loopback_LDFLAGS       = $(AM_LDFLAGS) $(RTMIDITESTLDFLAGS)
#%C%_errors_LDFLAGS         = $(AM_LDFLAGS) $(RTMIDITESTLDFLAGS)
#

%C%_midiprobe_LDADD      = $(RTMIDILIBRARYNAME)
%C%_midiout_LDADD        = $(RTMIDILIBRARYNAME)
%C%_qmidiin_LDADD        = $(RTMIDILIBRARYNAME)
%C%_cmidiin_LDADD        = $(RTMIDILIBRARYNAME)
%C%_sysextest_LDADD      = $(RTMIDILIBRARYNAME)
%C%_midiprobe2_LDADD     = $(RTMIDILIBRARYNAME)
%C%_midiprobe_all_LDADD  = $(RTMIDILIBRARYNAME)
%C%_cmidiin2_LDADD       = $(RTMIDILIBRARYNAME)
%C%_qmidiin2_LDADD       = $(RTMIDILIBRARYNAME)
%C%_midiout2_LDADD       = $(RTMIDILIBRARYNAME)
%C%_loopback_LDADD       = $(RTMIDILIBRARYNAME)
%C%_errors_LDADD         = $(RTMIDILIBRARYNAME)


if RTMIDICOPYDLLS

#-------------------------------------------------------------------------------------
# Installing DLLs
#-------------------------------------------------------------------------------------



rtmidi_installdll:
	@echo 'solving references for $(DLLLINKFILE)... '
	DLLSEARCHPATH="$(DLLSEARCHPATH)" ; \
	for f in `LANG=C $(OBJDUMP) -p  $(DLLEXEDIR)/$(DLLLINKFILE) |sed '/^\s*DLL Name:.*\(lib\|thread\|wx\|mingw\|gcc\|stdc++\)/ { s/^\s*DLL Name:\s*//; p } ; d '` ; \
	do \
		echo -n checking "$$f ... " ; \
		if [ ! -f $(DLLEXEDIR)/$$f ] ; then \
			echo -n "searching... " ; \
			IFS=: ; \
			for d in $$DLLSEARCHPATH ; do \
				echo "looking in $$d" ; \
				echo -n "found " ; \
				(test -f "$$d/$$f" && \
					echo "$$d/$$f" || \
					ls -1 "$$d/$$f.*" 2>/dev/null) ; \
				echo ; \
				IFS= ; \
				fullname=`(test -f "$$d/$$f" && \
					echo "$$d/$$f" || \
					ls -1 "$$d/$$f.*" 2>/dev/null) | \
					head -n1` ; \
				echo "testing $$fullname" ; \
				if test -f "$$fullname" ; \
				then \
					echo "installing $$fullname " ; \
					$(INSTALL_PROGRAM_ENV) $(INSTALL_DATA) "$$fullname" "`pwd`/$(DLLEXEDIR)" ; \
					case "$$f" in \
					*.gz)  GZIP=$(GZIP_ENV) gzip -dc $(DLLEXEDIR)/`basename "$$fullname"` >$(DLLEXEDIR)/"$$f" ;; \
					esac ; \
					$(MAKE) $(AM_MAKEFLAGS) DLLLINKFILE="$$f" DLLEXEDIR="$(DLLEXEDIR)" rtmidi_installdll ; \
				fi ; \
			done ; \
		fi ; \
		echo "done." ;\
	done ;

#-------------------------------------------------------------------------------------
# Certain fixed files
#-------------------------------------------------------------------------------------



check-am: check-dll-rtmidi

check-dll-rtmidi: all-am
	$(MAKE) $(AM_MAKEFLAGS) $(check_PROGRAMS)
	$(MAKE) $(AM_MAKEFLAGS)	linkchecks-rtmidi

linkchecks-rtmidi:
	for d in `echo $(DLLPROGRAMS)| tr ' ' '\n' | grep -i -e '$(EXEEXT)$$\|.dll$$'` ; \
	do \
		ls -l "$$d" ; \
		file=`basename "$$d"` ; \
		dir=`dirname "$$d"` ; \
		$(MAKE) $(AM_MAKEFLAGS) DLLLINKFILE="$$file" DLLEXEDIR="$$dir" rtmidi_installdll ; \
		test -f "$$dir/.libs/$$file" && \
			$(MAKE) $(AM_MAKEFLAGS) DLLLINKFILE="$$file" DLLEXEDIR="$$dir/.libs" rtmidi_installdll ; \
	done
endif
