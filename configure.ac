#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([h264dvr], [0.1], [scott@alfter.us])
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([lib/daemonize.cpp])
AC_CONFIG_HEADERS([config.h])
AC_CANONICAL_HOST
AM_CONDITIONAL(WIN32_BUILD, test "$host_os" = "mingw32")

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_RANLIB

# check for wxWidgets

WXCONFIG=wx-config
AC_ARG_WITH(wx-config,
[[  --with-wx-config=FILE     Use the given path to wx-config when determining
                            wxWidgets configuration; defaults to "wx-config"]],
[
    if test "$withval" != "yes" -a "$withval" != ""; then
        WXCONFIG=$withval
    fi
])
AC_MSG_CHECKING([wxWidgets version])
if wxversion=`$WXCONFIG --version`; then
    AC_MSG_RESULT([$wxversion])
else
    AC_MSG_RESULT([not found])
    AC_MSG_ERROR([wxWidgets is required. Try --with-wx-config.])
fi

WX_CPPFLAGS="`$WXCONFIG --cppflags`"
WX_CXXFLAGS="`$WXCONFIG --cxxflags | sed -e 's/-fno-exceptions//'`"
WX_LIBS="`$WXCONFIG --libs`"

CPPFLAGS="$CPPFLAGS $WX_CPPFLAGS"
CXXFLAGS="$CXXFLAGS $WX_CPPFLAGS"

AC_SUBST(WX_LIBS)


# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h stdlib.h string.h sys/ioctl.h sys/statvfs.h syslog.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_MMAP
AC_CHECK_FUNCS([clock_gettime memset])

# create top-level and subdirectory makefiles
AC_CONFIG_FILES([Makefile src/Makefile lib/Makefile include/Makefile])

AC_OUTPUT
