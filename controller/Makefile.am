pkgbin_PROGRAMS = jamincont_1912.so
pkgbindir = @prefix@/lib/ladspa
jamincont_1912_so_CFLAGS = @OSC_CFLAGS@
jamincont_1912_so_LDADD = @OSC_LIBS@

LINK = gcc -nostartfiles -shared -Wl,-Bsymbolic -lc -o $@

%.so.c: %.xml
	./xml2c.pl $*.xml > $*.so.c

.PRECIOUS:      %.so.c
