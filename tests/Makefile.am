### RtMidi tests Makefile - for various flavors of unix
#RTMIDITESTCXXFLAGS += -I$(top_srcdir)/%D%
#RTMIDITESTLDFLAGS  += -L$(top_builddir)/%D% -lrtmidi-ts @LIBS@

noinst_PROGRAMS += \
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

if HAVE_VIRTUAL_DEVICES
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
%C%_midiprobe_CXXFLAGS     = $(AM_CXXFLAGS) $(RTMIDITESTCXXFLAGS)
%C%_midiout_CXXFLAGS       = $(AM_CXXFLAGS) $(RTMIDITESTCXXFLAGS)
%C%_qmidiin_CXXFLAGS       = $(AM_CXXFLAGS) $(RTMIDITESTCXXFLAGS)
%C%_cmidiin_CXXFLAGS       = $(AM_CXXFLAGS) $(RTMIDITESTCXXFLAGS)
%C%_sysextest_CXXFLAGS     = $(AM_CXXFLAGS) $(RTMIDITESTCXXFLAGS)
%C%_midiprobe2_CXXFLAGS    = $(AM_CXXFLAGS) $(RTMIDITESTCXXFLAGS)
%C%_midiprobe_all_CXXFLAGS = $(AM_CXXFLAGS) $(RTMIDITESTCXXFLAGS)
%C%_cmidiin2_CXXFLAGS      = $(AM_CXXFLAGS) $(RTMIDITESTCXXFLAGS)
%C%_qmidiin2_CXXFLAGS      = $(AM_CXXFLAGS) $(RTMIDITESTCXXFLAGS)
%C%_midiout2_CXXFLAGS      = $(AM_CXXFLAGS) $(RTMIDITESTCXXFLAGS)
%C%_loopback_CXXFLAGS      = $(AM_CXXFLAGS) $(RTMIDITESTCXXFLAGS)
%C%_errors_CXXFLAGS        = $(AM_CXXFLAGS) $(RTMIDITESTCXXFLAGS)

%C%_midiprobe_LDFLAGS      = $(AM_LDFLAGS) $(RTMIDITESTLDFLAGS)
%C%_midiout_LDFLAGS        = $(AM_LDFLAGS) $(RTMIDITESTLDFLAGS)
%C%_qmidiin_LDFLAGS        = $(AM_LDFLAGS) $(RTMIDITESTLDFLAGS)
%C%_cmidiin_LDFLAGS        = $(AM_LDFLAGS) $(RTMIDITESTLDFLAGS)
%C%_sysextest_LDFLAGS      = $(AM_LDFLAGS) $(RTMIDITESTLDFLAGS)
%C%_midiprobe2_LDFLAGS     = $(AM_LDFLAGS) $(RTMIDITESTLDFLAGS)
%C%_midiprobe_all_LDFLAGS  = $(AM_LDFLAGS) $(RTMIDITESTLDFLAGS)
%C%_cmidiin2_LDFLAGS       = $(AM_LDFLAGS) $(RTMIDITESTLDFLAGS)
%C%_qmidiin2_LDFLAGS       = $(AM_LDFLAGS) $(RTMIDITESTLDFLAGS)
%C%_midiout2_LDFLAGS       = $(AM_LDFLAGS) $(RTMIDITESTLDFLAGS)
%C%_loopback_LDFLAGS       = $(AM_LDFLAGS) $(RTMIDITESTLDFLAGS)
%C%_errors_LDFLAGS         = $(AM_LDFLAGS) $(RTMIDITESTLDFLAGS)


if RTMIDICOPYDLLS

#-------------------------------------------------------------------------------------
# Installing DLLs
#-------------------------------------------------------------------------------------


rtmidi_installdll:
	@echo 'solving references for $(DLLLINKFILE)... '
	DLLSEARCHPATH="$(DLLSEARCHPATH)" ; \
	for d in `LANG=C $(OBJDUMP) -p  $(DLLEXEDIR)/$(DLLLINKFILE) |sed '/^\s*DLL Name:.*\(lib\|thread\|wx\|mingw\|gcc\|stdc++\)/ { s/^\s*DLL Name:\s*//; p } ; d '` ; \
	do \
		echo -n checking "$$d ... " ; \
		if [ ! -f $(DLLEXEDIR)/$$d ] ; then \
			echo -n "searching... " ; \
			f=`( find $$DLLSEARCHPATH -name "$$d" || \
			find $$DLLSEARCHPATH -name "$$d.*")|head -n 1` ; \
			if test -f "$$f" ; \
			then \
				echo "installing $$f " ; \
				$(INSTALL_PROGRAM_ENV) $(INSTALL_DATA) "$$f" "`pwd`/$(DLLEXEDIR)" ; \
				case "$$f" in \
				*.gz)  GZIP=$(GZIP_ENV) gzip -dc $(DLLEXEDIR)/`basename "$$f"` >$(DLLEXEDIR)/"$$d" ;; \
				esac ; \
				$(MAKE) $(AM_MAKEFLAGS) DLLLINKFILE="$$d" DLLEXEDIR="$(DLLEXEDIR)" rtmidi_installdll ; \
			else \
				echo "not found." ; \
			fi ; \
		fi ; \
		echo "done." ;\
	done ;

#-------------------------------------------------------------------------------------
# Certain fixed files
#-------------------------------------------------------------------------------------



check-am: check-dll

check-dll: all-am
	$(MAKE) $(AM_MAKEFLAGS) $(check_PROGRAMS)
	$(MAKE) $(AM_MAKEFLAGS)	linkchecks

linkchecks:
	for d in `echo $(TESTS)| tr ' ' '\n' | grep -i -e '$(EXEEXT)$$'` ; \
	do \
		ls -l "$$d" ; \
		$(MAKE) $(AM_MAKEFLAGS) DLLLINKFILE="$$d" DLLEXEDIR="." rtmidi_installdll ; \
		test -f ".libs/$$d" && \
			$(MAKE) $(AM_MAKEFLAGS) DLLLINKFILE=".libs/$$d" DLLEXEDIR="." rtmidi_installdll ; \
	done
endif
