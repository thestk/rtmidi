# AC_LIB_COREMIDI(FUNCTION,
#              [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND],
#              [OTHER-LIBRARIES])
# ------------------------------------------------------
#
# Use a cache variable name containing both the library and function name,
# because the test really is for library $1 defining function $2, not
# just for library $1.  Separate tests with the same $1 and different $2s
# may have different results.
#
# Note that using directly AS_VAR_PUSHDEF([ac_Lib], [ac_cv_lib_$1_$2])
# is asking for troubles, since AC_CHECK_LIB($lib, fun) would give
# ac_cv_lib_$lib_fun, which is definitely not what was meant.  Hence
# the AS_LITERAL_IF indirection.
#
# FIXME: This macro is extremely suspicious.  It DEFINEs unconditionally,
# whatever the FUNCTION, in addition to not being a *S macro.  Note
# that the cache does depend upon the function we are looking for.
#
# It is on purpose we used `ac_check_lib_save_LIBS' and not just
# `ac_save_LIBS': there are many macros which don't want to see `LIBS'
# changed but still want to use AC_CHECK_LIB, so they save `LIBS'.
# And ``ac_save_LIBS' is too tempting a name, so let's leave them some
# freedom.
AC_DEFUN([RTMIDI_LIB_COREMIDI],[
	AC_LANG_PUSH(C++)
	ac_cv_rtmidi_lib_coremidi="$LIBS"
	LIBS="$LIBS -framework CoreMIDI -framework CoreFoundation -framework CoreAudio"
	rtmidi_have_coremidi=no
	AC_CHECK_HEADER(CoreMIDI/CoreMIDI.h, [rtmidi_have_coremidi=yes])
	AS_IF(test "x$rtmidi_have_coremidi" = "xyes",[
		AC_CHECK_LIB(pthread, pthread_create, ,[
		     rtmidi_have_coremidi=no
		     AC_MSG_WARN([RtMidi requires the pthread library!])
		])
	])
	AS_IF(test "x$rtmidi_have_coremidi" = "xyes",[
		RTMIDI_API="$RTMIDI_API -D__MACOSX_COREMIDI__"
		RTMIDI_LIBS="$RTMIDI_LIBS -framework CoreMIDI -framework CoreFoundation -framework CoreAudio"
		$1
	], [
		m4_default([$2],[AC_MSG_ERROR(CoreMIDI header files not found!)] )
	])
	LIBS="$ac_cv_rtmidi_lib_coremidi"
	AC_LANG_POP(C++)
])dnl RTMIDI_LIB_COREMIDI
