# Makefile.am -- Process this file with automake to produce Makefile.in

AM_CXXFLAGS = -Wall -Werror

AM_CPPFLAGS = \
	-I$(srcdir)/src/common \
	-I$(srcdir)/libraries/excmd/src \
	-I$(srcdir)/libraries/fmt/include


LDADD = libfmt.la


if PLATFORM_MINGW
AM_CPPFLAGS += -D__USE_MINGW_ANSI_STDIO=1
endif PLATFORM_MINGW


if PLATFORM_APPLE
LDADD += -lc++
endif

noinst_HEADERS = \
	libraries/excmd/src/excmd.h		\
	libraries/excmd/src/excmd_exception.h	\
	libraries/excmd/src/excmd_meta.h	\
	libraries/excmd/src/excmd_str.h		\
	libraries/excmd/src/excmd_value_parser.h \
	src/common/be_val.h			\
	src/common/elf.h			\
	src/common/rplwrap.h			\
	src/common/type_traits.h		\
	src/common/utils.h


bin_PROGRAMS = \
	elf2rpl		\
	readrpl		\
	rplexportgen	\
	rplimportgen	\
	udplogserver

if BUILD_WUHBTOOL
bin_PROGRAMS += wuhbtool
endif BUILD_WUHBTOOL


noinst_LTLIBRARIES = libfmt.la


libfmt_la_SOURCES = \
	libraries/fmt/include/fmt/args.h	\
	libraries/fmt/include/fmt/base.h	\
	libraries/fmt/include/fmt/chrono.h	\
	libraries/fmt/include/fmt/color.h	\
	libraries/fmt/include/fmt/compile.h	\
	libraries/fmt/include/fmt/core.h	\
	libraries/fmt/include/fmt/format.h	\
	libraries/fmt/include/fmt/format-inl.h	\
	libraries/fmt/include/fmt/os.h		\
	libraries/fmt/include/fmt/ostream.h	\
	libraries/fmt/include/fmt/printf.h	\
	libraries/fmt/include/fmt/ranges.h	\
	libraries/fmt/include/fmt/std.h		\
	libraries/fmt/include/fmt/xchar.h	\
	libraries/fmt/src/format.cc		\
	libraries/fmt/src/os.cc


elf2rpl_SOURCES	= src/elf2rpl/main.cpp

elf2rpl_CPPFLAGS = \
	$(AM_CPPFLAGS) \
	$(ZLIB_CFLAGS)

elf2rpl_LDADD = \
	$(ZLIB_LIBS) \
	$(LDADD)


readrpl_SOURCES = \
	src/readrpl/generate_exports_def.cpp	\
	src/readrpl/generate_exports_def.h	\
	src/readrpl/main.cpp			\
	src/readrpl/print.cpp			\
	src/readrpl/print.h			\
	src/readrpl/readrpl.h			\
	src/readrpl/verify.cpp			\
	src/readrpl/verify.h


readrpl_LDADD = \
	$(ZLIB_LIBS) \
	$(LDADD)


rplexportgen_SOURCES = src/rplexportgen/rplexportgen.cpp

rplexportgen_LDADD = \
	$(ZLIB_LIBS) \
	$(LDADD)


rplimportgen_SOURCES = src/rplimportgen/rplimportgen.cpp

rplimportgen_CPPFLAGS = \
	$(AM_CPPFLAGS) \
	$(ZLIB_CFLAGS)

rplimportgen_LDADD = \
	$(ZLIB_LIBS) \
	$(LDADD)


udplogserver_SOURCES = src/udplogserver/main.cpp

udplogserver_LDADD = $(LDADD)

if PLATFORM_MINGW
udplogserver_LDADD += -lws2_32
endif PLATFORM_MINGW


if BUILD_WUHBTOOL

wuhbtool_SOURCES = \
	src/wuhbtool/entities/BufferFileEntry.cpp	\
	src/wuhbtool/entities/BufferFileEntry.h		\
	src/wuhbtool/entities/DirectoryEntry.cpp	\
	src/wuhbtool/entities/DirectoryEntry.h		\
	src/wuhbtool/entities/FileEntry.cpp		\
	src/wuhbtool/entities/FileEntry.h		\
	src/wuhbtool/entities/NodeEntry.cpp		\
	src/wuhbtool/entities/NodeEntry.h		\
	src/wuhbtool/entities/OSFileEntry.cpp		\
	src/wuhbtool/entities/OSFileEntry.h		\
	src/wuhbtool/entities/RootEntry.cpp		\
	src/wuhbtool/entities/RootEntry.h		\
	src/wuhbtool/main.cpp				\
	src/wuhbtool/services/RomFSService.cpp		\
	src/wuhbtool/services/RomFSService.h		\
	src/wuhbtool/services/RomFSStructs.h		\
	src/wuhbtool/services/TgaGzService.cpp		\
	src/wuhbtool/services/TgaGzService.h		\
	src/wuhbtool/utils/filepath.cpp			\
	src/wuhbtool/utils/filepath.h			\
	src/wuhbtool/utils/types.h			\
	src/wuhbtool/utils/utils.h

wuhbtool_CPPFLAGS = \
	$(AM_CPPFLAGS) \
	$(FREEIMAGE_CPPFLAGS) \
	$(ZLIB_CFLAGS)

wuhbtool_LDADD = \
	$(FREEIMAGE_LDFLAGS) -lfreeimage \
	$(ZLIB_LIBS) \
	$(LDADD)

endif BUILD_WUHBTOOL


EXTRA_DIST = \
	bootstrap \
	LICENSE.md
