## Process this file with automake to produce Makefile.in

INCLUDES = \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	@PACKAGE_CFLAGS@

jamin_CFLAGS = -g -ffast-math
jamin_LDFLAGS = -lm

bin_PROGRAMS = jamin

jamin_SOURCES = \
	callbacks.c callbacks.h \
	compressor.h \
	compressor-ui.c compressor-ui.h \
	db.h \
	debug.h \
	geq.c geq.h \
	gtkmeter.c gtkmeter.h \
	gtkmeterscale.c gtkmeterscale.h \
	interface.c interface.h \
	intrim.c intrim.h \
	io.c io.h \
	jackstatus.h \
	limiter.h \
	limiter-ui.c limiter-ui.h \
	main.c main.h \
	plugin.c plugin.h \
	process.c process.h \
	ringbuffer.c ringbuffer.h \
	spectrum.c spectrum.h \
	spline.c \
	state.c state.h \
	stereo.c stereo.h \
	support.c support.h \
	transport.c transport.h

jamin_LDADD = @PACKAGE_LIBS@

state.h: mk-stateh.pl state-vars.txt
	@echo "WARNING: Rebuilding state.h"
	./mk-stateh.pl

EXTRA_DIST = mk-stateh.pl

quickly:
	(cd .. && make quickly)
