AC_DEFUN([AC_CHECK_DAQCAP_HEADERS],
[
    AC_ARG_WITH(daqcap_includes,
                AS_HELP_STRING([--with-daqcap-includes=DIR],[daqcap include directory]),
                [], [with_daqcap_includes="no"])

    if test "x$with_daqcap_includes" != "xno"; then
        DAQCAP_CPPFLAGS="-I${with_daqcap_includes}"
    fi

    save_CPPFLAGS="$CPPFLAGS"
    CPPFLAGS="$CPPFLAGS $DAQCAP_CPPFLAGS $LIBDAQ_CPPFLAGS"
    AC_CHECK_HEADER([daq_capture.h], [HAVE_DAQCAP_HEADERS="yes"], [HAVE_DAQCAP_HEADERS="no"])
    CPPFLAGS="$save_CPPFLAGS"
])

AC_DEFUN([AC_CHECK_DAQCAP_LIBS],
[
    AC_ARG_WITH(daqcap_libraries,
                AS_HELP_STRING([--with-daqcap-libraries=DIR],[daqcap library directory]),
                [], [with_daqcap_libraries="no"])

    if test "x$with_daqcap_libraries" != "xno"; then
        DAQCAP_LDFLAGS="-L${with_daqcap_libraries}"
    fi

    save_LDFLAGS="$LDFLAGS"
    save_LIBS="$LIBS"
    LDFLAGS="$LDFLAGS $DAQCAP_LDFLAGS"
    AC_CHECK_LIB([daqcap], [daq_capture_open_writer], [HAVE_DAQCAP_LIBRARIES="yes"], [HAVE_DAQCAP_LIBRARIES="no"])
    LIBS="$save_LIBS"
    LDFLAGS="$save_LDFLAGS"
])
