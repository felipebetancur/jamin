pkgbin_PROGRAMS = jamincont_1912.so
pkgbindir = @prefix@/lib/ladspa
jamincont_1912_so_CFLAGS = @OSC_CFLAGS@ -I../src
jamincont_1912_so_LDADD = @OSC_LIBS@

EXTRA_DIST = jamincont_1912.xml ladspa-swh.dtd xml2c.pl
LINK = gcc -nostartfiles -shared -Wl,-Bsymbolic -lc -o $@

%.c: %.xml
	./xml2c.pl $*.xml > $*.c

.PRECIOUS:      %.c
