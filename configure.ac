#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.65)
AC_INIT([libzakcgi], [0.1.0], [azagli@libero.it])
AC_CONFIG_SRCDIR([src/main.c])
AC_CONFIG_HEADER([config.h])

AM_INIT_AUTOMAKE(-Wall)

m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])

AM_MAINTAINER_MODE

AC_CONFIG_MACRO_DIR([m4])

AC_CANONICAL_SYSTEM

LT_INIT

AC_LIBTOOL_WIN32_DLL

# Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL
AC_PROG_RANLIB

# Checks for libraries.
PKG_CHECK_MODULES(ZAKCGI, [glib-2.0 >= 2.36
                           gobject-2.0 >= 2.36
                           gio-2.0 >= 2.36
                           gio-unix-2.0 >= 2.36])

AC_SUBST(ZAKCGI_CFLAGS)
AC_SUBST(ZAKCGI_LIBS)

# Checks for header files.
AC_HEADER_STDC

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

# Checks for library functions.

# Checks for library functions.

dnl ******************************
dnl Check for Operating System
dnl ******************************

platform_win32=no

case "$host" in
*-mingw*)
    platform_win32=yes
    ;;
esac

AM_CONDITIONAL(PLATFORM_WIN32, [test $platform_win32 = yes])

# Output files
AC_CONFIG_FILES([
	libzakcgi.pc
	Makefile
	src/Makefile
	tests/Makefile
])
AC_OUTPUT
