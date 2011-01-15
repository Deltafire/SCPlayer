# Process this file with automake to generate a Makefile
#
SUBDIRS = src
ACLOCAL_AMFLAGS = -I m4
LIBTOOL_DEPS = @LIBTOOL_DEPS@
libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status libtool
dist-hook:
	git --git-dir ${srcdir}/.git rev-parse HEAD > ${distdir}/git_revision
