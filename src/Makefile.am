## Process this file with automake to produce Makefile.in

INCLUDES = \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	@PACKAGE_CFLAGS@

jamin_CFLAGS = -g -ffast-math
jamin_LDFLAGS = -lm

bin_PROGRAMS = jamin

man_MANS = jamin.1

jamin_SOURCES = \
	callbacks.c callbacks.h callbacks_help.h \
	compressor.h \
	compressor-ui.c compressor-ui.h \
	db.h \
	debug.h \
	geq.c geq.h \
	hdeq.c hdeq.h\
	gtkmeter.c gtkmeter.h \
	gtkmeterscale.c gtkmeterscale.h \
	interface.c interface.h \
	intrim.c intrim.h \
	io.c io.h \
	io-menu.c io-menu.h \
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
	status-ui.c status-ui.h \
	stereo.c stereo.h \
	support.c support.h \
	transport.c transport.h \
	scenes.c scenes.h \
	help.c help.h
	status-ui.c status-ui.h

jamin_LDADD = @PACKAGE_LIBS@

state.h: mk-stateh.pl state-vars.txt
	@echo "WARNING: Rebuilding state.h"
	./mk-stateh.pl

EXTRA_DIST = mk-stateh.pl jamin.1

quickly:
	(cd .. && make quickly)
