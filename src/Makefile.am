SUBDIRS = libretro-cores/Dinothawr

bin_PROGRAMS = emulatron


BUILT_SOURCES = emu-resources.h emu-resources.c
EXTRA_DIST = emu.gresource.xml emu-header-bar.ui emulatron.ui

emu-resources.h: emu.gresource.xml
	$(AM_V_GEN) $(GLIB_COMPILE_RESOURCES) $(srcdir)/emu.gresource.xml \
		--target=$@ --sourcedir=$(srcdir) --c-name _emulatron --generate-header
emu-resources.c: $(EXTRA_DIST) emu.gresource.xml
	$(AM_V_GEN) $(GLIB_COMPILE_RESOURCES) $(srcdir)/emu.gresource.xml \
		--target=$@ --sourcedir=$(srcdir) --c-name _emulatron --generate-source

emulatron_LDADD = \
	$(PLAYER_LIBS)

emulatron_CFLAGS = \
	$(PLAYER_CFLAGS) \
	$(AM_CPPFLAGS)

emulatron_CPPFLAGS = \
	$(PLAYER_CFLAGS) \
	$(AM_CFLAGS)

emulatron_SOURCES = \
	emulatron.cc \
	emulatron.hh \
	emu-window.cc \
	emu-window.hh \
	emu-header-bar.cc \
	emu-header-bar.hh \
	emu-resources.h \
	emu-resources.c

