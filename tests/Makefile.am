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

if COPYDLLS
check-am: check-dll

check-dll: all-am
	$(MAKE) $(AM_MAKEFLAGS) $(check_PROGRAMS)
	$(MAKE) $(AM_MAKEFLAGS)	linkchecks

linkchecks:
	for d in `echo $(TESTS)| tr ' ' '\n' | grep -i -e '$(EXEEXT)$$'` ; \
	do \
		ls -l "$$d" ; \
		$(MAKE) $(AM_MAKEFLAGS) DLLLINKFILE="$$d" DLLEXEDIR="." installdll ; \
		test -f ".libs/$$d" && \
			$(MAKE) $(AM_MAKEFLAGS) DLLLINKFILE=".libs/$$d" DLLEXEDIR="." installdll ; \
	done

installdll:
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
				$(MAKE) DLLLINKFILE="$$d" DLLEXEDIR="$(DLLEXEDIR)" installdll ; \
			else \
				echo "not found." ; \
			fi ; \
		fi ; \
		echo "done." ;\
	done ;

endif

