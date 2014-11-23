SUBDIRS += src/libretro-cores/Dinothawr

bin_PROGRAMS += src/emulatron


EXTRA_DIST += src/resources/emu.gresource.xml.in

emu_resource_deps = $(shell $(GLIB_COMPILE_RESOURCES) --generate-dependencies --sourcedir $(srcdir)/src/resources --sourcedir $(builddir)/src/resources $(builddir)/src/resources/emu.gresource.xml)

emu_resource_c_deps = $(builddir)/src/resources/emu.gresource.xml $(emu_resource_deps)

$(info $(emu_resource_deps))

src/emu-resources.c: $(emu_resource_c_deps)
	$(AM_V_GEN) $(GLIB_COMPILE_RESOURCES) \
		--target="$@" \
		--sourcedir="$(srcdir)/src/resources" \
		--sourcedir="$(builddir)/src/resources" \
		--generate-source "$(builddir)/src/resources/emu.gresource.xml"

emu_dist_resource_deps = \
	$(filter-out src/resources/css/emu-style-os.css,$(filter-out src/resources/gtk/menu.ui,$(gedit_resource_deps))) \
	src/resources/gtk/menus-default.ui \
	src/resources/gtk/menus-default.ui \
	src/resources/gtk/menus-osx.ui

EXTRA_DIST += \
	$(emu_dist_resource_deps)

src_emulatron_LDADD = \
	$(PLAYER_LIBS)

src_emulatron_CFLAGS = \
	$(PLAYER_CFLAGS) \
	$(AM_CPPFLAGS)

src_emulatron_CPPFLAGS = \
	$(PLAYER_CFLAGS) \
	$(AM_CFLAGS)

src_emulatron_SOURCES = \
	src/emulatron.cc \
	src/emulatron.hh \
	src/emu-window.cc \
	src/emu-window.hh \
	src/game-view.cc \
	src/game-view.hh \
	src/game-store.cc \
	src/game-store.hh \
	src/game-model.cc \
	src/game-model.hh \
	src/emu-about-dialog.cc \
	src/emu-about-dialog.hh \
	src/emu-preference-window.cc \
	src/emu-preference-window.hh \
	src/emu-header-bar.cc \
	src/emu-header-bar.hh \
	src/emu-resources.c

BUILT_SOURCES += $(emu_built_sources) src/emu-resources.c
