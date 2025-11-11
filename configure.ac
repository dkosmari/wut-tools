#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.71)
AC_INIT([WUT Tools],
        [1.3.5+],
        [https://github.com/dkosmari/wut-tools/issues],
        [],
        [https://github.com/dkosmari/wut-tools])
AC_CONFIG_SRCDIR([src/elf2rpl/main.cpp])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_HEADERS([config.h])

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


AX_CHECK_LIBRARY([FREEIMAGE], [FreeImage.h], [freeimage],
                 [AS_VAR_SET([FOUND_FREEIMAGE], [yes])],
                 [AS_VAR_SET([FOUND_FREEIMAGE], [no])])
AS_VAR_IF([FOUND_FREEIMAGE], [no],
          [AC_MSG_WARN([FreeImage not found, will not build wuhbtool!])])
AM_CONDITIONAL([BUILD_WUHBTOOL], [test $FOUND_FREEIMAGE = yes])


PLATFORM=
AS_CASE([$host],
        [*-*-mingw*], [AS_VAR_SET([PLATFORM], [mingw])],
        [*-apple-darwin*], [AS_VAR_SET([PLATFORM], [apple])],
        [AS_VAR_SET([PLATFORM], [generic])])
AM_CONDITIONAL([PLATFORM_MINGW], [test $PLATFORM = mingw])
AM_CONDITIONAL([PLATFORM_APPLE], [test $PLATFORM = apple])
AC_MSG_NOTICE([Detected platform: $PLATFORM])


AC_CONFIG_FILES([Makefile])
AC_OUTPUT
