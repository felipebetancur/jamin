## Process this file with automake to produce Makefile.in

INCLUDES = \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	@PACKAGE_CFLAGS@

jamin_CFLAGS = -g -ffast-math
jamin_LDFLAGS = -lm

bin_PROGRAMS = jamin

# EXTRA_DIST = *.h

jamin_SOURCES = \
	main.c main.h \
	support.c support.h \
	interface.c interface.h \
	callbacks.c callbacks.h \
	geq.c geq.h \
	io.c io.h \
	spline.c \
	process.c process.h \
	plugin.c plugin.h \
	compressor.h \
	limiter.h \
	intrim.c intrim.h \
	limiter-ui.c limiter-ui.h \
	compressor-ui.c compressor-ui.h \
	gtkmeter.c gtkmeter.h

jamin_LDADD = @PACKAGE_LIBS@
