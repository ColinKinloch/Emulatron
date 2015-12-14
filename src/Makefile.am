#SUBDIRS += src/libretro-cores/Dinothawr
#SUBDIRS += src/libretro-cores/bsnes-libretro
#$(shell cp src/libretro-cores/vba-next/Makefile.libretro \
#	   src/libretro-cores/vba-next/Makefile)
#SUBDIRS += src/libretro-cores/vba-next
#SUBDIRS += src/libretro-cores/mupen64plus-libretro
#SUBDIRS += src/libretro-cores/snes9x/libretro

bin_PROGRAMS += emulatron


EXTRA_DIST += src/resources/emu.gresource.xml.in

system_resource_deps = \
	$(wildcard $(srcdir)/src/resources/systems/$(1)/$(1)-system.gresource.xml) \
	$(shell $(GLIB_COMPILE_RESOURCES) --generate-dependencies \
		--sourcedir $(srcdir)/src/resources/systems/$(1) \
		--sourcedir $(builddir)/src/resources/systems/$(1) \
		$(builddir)/src/resources/systems/$(1)/$(1)-system.gresource.xml)

emu_resource_deps = \
	$(shell $(GLIB_COMPILE_RESOURCES) --generate-dependencies \
		--sourcedir $(srcdir)/src/resources \
		--sourcedir $(builddir)/src/resources \
		$(builddir)/src/resources/emu.gresource.xml)


emu_resource_c_deps = \
	$(builddir)/src/resources/emu.gresource.xml \
	$(emu_resource_deps)

#systems_resource_c_deps = \
#	$(builddir)/src/resources/systems/systems.gresource.xml \
#	$(systems_resource_deps)

#$(info $(emu_resource_deps))
src/emu-resources.h:
	$(AM_V_GEN) $(GLIB_COMPILE_RESOURCES) \
		--target="$@" \
		--sourcedir="$(srcdir)/src/resources" \
		--sourcedir="$(builddir)/src/resources" \
		--generate-header "$(builddir)/src/resources/emu.gresource.xml"

src/emu-resources.c: $(emu_resource_c_deps)
	$(AM_V_GEN) $(GLIB_COMPILE_RESOURCES) \
		--target="$@" \
		--sourcedir="$(srcdir)/src/resources" \
		--sourcedir="$(builddir)/src/resources" \
		--generate-source "$(builddir)/src/resources/emu.gresource.xml"

.SECONDEXPANSION:
src/%-system-resources.h: $$(call system_resource_deps,%)
	$(AM_V_GEN) $(GLIB_COMPILE_RESOURCES) \
		--target="$@" \
		--sourcedir="$(srcdir)/src/resources/systems/$(@:src/%-system-resources.h=%)" \
		--sourcedir="$(builddir)/src/resources/systems/$(@:src/%-system-resources.h=%)" \
		--generate-header "$(builddir)/src/resources/systems/$(@:src/%-system-resources.h=%)/$(@:src/%-system-resources.h=%)-system.gresource.xml"

.SECONDEXPANSION:
src/%-system-resources.c: $$(call system_resource_deps,%)
	$(AM_V_GEN) $(GLIB_COMPILE_RESOURCES) \
		--target="$@" \
		--sourcedir="$(srcdir)/src/resources/systems/$(@:src/%-system-resources.c=%)" \
		--sourcedir="$(builddir)/src/resources/systems/$(@:src/%-system-resources.c=%)" \
		--generate-source "$(builddir)/src/resources/systems/$(@:src/%-system-resources.c=%)/$(@:src/%-system-resources.c=%)-system.gresource.xml"

emu_dist_resource_deps = \
	$(filter-out src/resources/css/emu-style-os.css,$(filter-out src/resources/gtk/menu.ui,$(emu_resource_deps))) \
	src/resources/gtk/menus-default.ui \
	src/resources/gtk/menus-default.ui \
	src/resources/gtk/menus-osx.ui

EXTRA_DIST += \
	$(emu_dist_resource_deps)

emulatron_LDADD = \
	$(PLAYER_LIBS)

emulatron_CFLAGS = \
	$(PLAYER_CFLAGS) \
	$(AM_CFLAGS)

emulatron_CPPFLAGS = \
	$(PLAYER_CFLAGS) \
	$(PLAYER_CPPFLAGS) \
	$(AM_CFLAGS) \
	$(AM_CPPFLAGS)

emulatron_SOURCES = \
	src/emulatron.cc \
	src/emulatron.hh \
	src/retro/core.cc \
	src/retro/core.hh \
	src/retro/console.cc \
	src/retro/console.hh \
	src/audio.cc \
	src/audio.hh \
	src/openvgdb.cc \
	src/openvgdb.hh \
	src/emu-window.cc \
	src/emu-window.hh \
	src/game-icon-view.cc \
	src/game-icon-view.hh \
	src/game-tree-view.cc \
	src/game-tree-view.hh \
	src/game-store.cc \
	src/game-store.hh \
	src/game-model.hh \
	src/console-store.cc \
	src/console-store.hh \
	src/console-model.hh \
	src/mouse.cc \
	src/mouse.hh \
	src/controller.cc \
	src/controller.hh \
	src/collection-store.cc \
	src/collection-store.hh \
	src/collection-model.hh \
	src/controller-store.cc \
	src/controller-store.hh \
	src/controller-model.hh \
	src/player-store.cc \
	src/player-store.hh \
	src/player-model.hh \
	src/emu-about-dialog.cc \
	src/emu-about-dialog.hh \
	src/emu-preference-window.cc \
	src/emu-preference-window.hh \
	src/control-pref-panel.cc \
	src/control-pref-panel.hh \
	src/emu-header-bar.cc \
	src/emu-header-bar.hh \
	src/emu-resources.c \
	src/emu-resources.h \
	src/Default-system-resources.c \
	src/3DO-system-resources.c \
	src/S32X-system-resources.c \
	src/A2600-system-resources.c \
	src/A5200-system-resources.c \
	src/A7800-system-resources.c \
	src/ColecoVision-system-resources.c \
	src/FDS-system-resources.c \
	src/GB-system-resources.c \
	src/GBA-system-resources.c \
	src/GC-system-resources.c \
	src/GG-system-resources.c \
	src/Intellivision-system-resources.c \
	src/Jaguar-system-resources.c \
	src/Lynx-system-resources.c \
	src/MAME-system-resources.c \
	src/MD-system-resources.c \
	src/N64-system-resources.c \
	src/NDS-system-resources.c \
	src/NES-system-resources.c \
	src/NGP-system-resources.c \
	src/PCE-system-resources.c \
	src/PCECD-system-resources.c \
	src/PCFX-system-resources.c \
	src/PSP-system-resources.c \
	src/PSX-system-resources.c \
	src/Saturn-system-resources.c \
	src/SCD-system-resources.c \
	src/SG1000-system-resources.c \
	src/SMS-system-resources.c \
	src/SNES-system-resources.c \
	src/VB-system-resources.c \
	src/Vectrex-system-resources.c \
	src/VMU-system-resources.c \
	src/WonderSwan-system-resources.c
#	src/game-area.cc \
#	src/game-area.hh \
#	src/$(systems)-system-resources.c
#	$(addprefix src/,$(addsuffix -system-resources.c,$(notdir $(wildcard src/resources/systems/*))))
#	src/Default-system-resources.h
#	$(addprefix src/,$(addsuffix -system-resources.h,$(notdir $(wildcard src/resources/systems/*))))

$(info $(emulatron_SOURCES))

BUILT_SOURCES += $(emu_built_sources) \
	src/emu-resources.c \
	src/emu-resources.h \
	$(addprefix src/,$(addsuffix -system-resources.c,$(notdir $(wildcard src/resources/systems/*))))
#	$(addprefix src/,$(addsuffix -system-resources.h,$(notdir $(wildcard src/resources/systems/*))))

