## Process this file with automake to produce Makefile.in

INCLUDES = \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	@PACKAGE_CFLAGS@

jam_CFLAGS = -g -ffast-math

bin_PROGRAMS = jam

# EXTRA_DIST = *.h

jam_SOURCES = \
	main.c main.h \
	support.c support.h \
	interface.c interface.h \
	callbacks.c callbacks.h \
	geq.c geq.h \
	io.c io.h \
	process.c process.h \
	plugin.c plugin.h \
	compressor.h \
	limiter.h \
	intrim.c intrim.h \
	limiter-ui.c limiter-ui.h \
	compressor-ui.c compressor-ui.h


jam_LDADD = @PACKAGE_LIBS@

jam_LDFLAGS = -lm
