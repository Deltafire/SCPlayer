# Process this file with automake to generate a Makefile
#
SUBDIRS = src
EXTRA_DIST = songs
ACLOCAL_AMFLAGS = -I m4
LIBTOOL_DEPS = @LIBTOOL_DEPS@
libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status libtool
