# Process this file with automake to generate a Makefile
#
bin_PROGRAMS     = scplay
scplay_SOURCES   = scplay.cpp
scplay_CXXFLAGS    = ${SDL_CFLAGS} -I${srcdir}/../scplayerlib
scplay_LDADD     = ../scplayerlib/libscplayer.la @SDL_LIBS@

LIBTOOL_DEPS     = @LIBTOOL_DEPS@
libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status libtool
