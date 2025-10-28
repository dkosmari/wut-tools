#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.71)
AC_INIT([WUT Tools],
        [1.3.5],
        [https://github.com/devkitPro/wut-tools/issues],
        [],
        [https://github.com/devkitPro/wut-tools])
AC_CONFIG_SRCDIR([src/elf2rpl/main.cpp])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_HEADER([config.h])

AC_CANONICAL_HOST

AM_INIT_AUTOMAKE([foreign subdir-objects])

LT_INIT

AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_AWK
AC_PROG_CPP
AC_PROG_LN_S
AC_PROG_MAKE_SET

AC_SYS_LARGEFILE

AC_LANG([C++])
AX_CXX_COMPILE_STDCXX([20], [noext], [mandatory])


PKG_CHECK_MODULES([ZLIB], [zlib])


AX_CHECK_LIBRARY([FREEIMAGE], [FreeImage.h], [freeimage])


PLATFORM=
AS_CASE([$host],
        [*-*-mingw*], [PLATFORM=mingw],
        [*-apple-darwin*], [PLATFORM=apple],
        [])
AM_CONDITIONAL([PLATFORM_MINGW], [test "x$PLATFORM" = xmingw])
AM_CONDITIONAL([PLATFORM_APPLE], [test "x$PLATFORM" = xapple])


AC_CONFIG_FILES([Makefile])
AC_OUTPUT
