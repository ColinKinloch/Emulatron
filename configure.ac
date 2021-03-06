# AC_PREREQ[(2.64)]

m4_define(emulatron_version_major, 0)
m4_define(emulatron_version_minor, 0)
m4_define(emulatron_version_micro, 0)

AC_INIT([emulatron],
        [emulatron_version_major.emulatron_version_minor.emulatron_version_micro],
        [colin@kinlo.ch],
        [emulatron],
        [http://colin.kinlo.ch/emulatron])

AC_CONFIG_SRCDIR([src/emulatron.cc])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([build-aux])

AM_INIT_AUTOMAKE([foreign -Wall -Werror -Wno-portability subdir-objects])

GETTEXT_PACKAGE=emulatron
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, ["$GETTEXT_PACKAGE"], [Define to the Gettext package name])
AC_SUBST(GETTEXT_PACKAGE)

IT_PROG_INTLTOOL([0.50.1])

AM_GNU_GETTEXT_VERSION([0.19.6])
AM_GNU_GETTEXT([external])

GOBJECT_INTROSPECTION_CHECK([0.6.7])
m4_define([glib_required_version], [2.23.0])
AM_PATH_GLIB_2_0(glib_required_version)

AC_PROG_CC()
AC_PROG_CXX()
AC_LANG([C++])
AX_CXX_COMPILE_STDCXX_11
AC_CANONICAL_HOST

# Initialize libtool
LT_PREREQ([2.2])
LT_INIT
m4_ifdef([LT_OUTPUT], [LT_OUTPUT])

PKG_PROG_PKG_CONFIG

AC_PATH_PROG([GLIB_MKENUMS],[glib-mkenums])


# Requirements
GLIB_REQS=2.35.0
GIO_REQS=2.27.92
GTK_REQS=3.16.0

# Used in the pkg-config file for plugins
AC_SUBST(GLIB_REQS)
AC_SUBST(GTK_REQS)
AC_SUBST(GIO_REQS)

EMULATRON_VERSION_MAJOR=emulatron_version_major
EMULATRON_VERSION_MINOR=emulatron_version_minor
EMULATRON_VERSION_MICRO=emulatron_version_micro
AC_SUBST(EMULATRON_VERSION_MAJOR)
AC_SUBST(EMULATRON_VERSION_MINOR)
AC_SUBST(EMULATRON_VERSION_MICRO)

#LIBRETRO_CORES=""

#PKG_CHECK_MODULES(LIBRETRO, $LIBRETRO_CORES)

AC_SEARCH_LIBS([dlopen], [dl dld zrldsrl])
PKG_CHECK_MODULES(PLAYER, gtkmm-3.0 giomm-2.4 libgdamm-5.0 epoxy sdl2 libpulse libgsf-1)

AC_PATH_PROG(GLIB_COMPILE_RESOURCES, glib-compile-resources)

if test "$os_osx" = "yes"; then
  os_menus_ui=menus-osx.ui
  os_style_css=emu-style-osx.css
else
  os_menus_ui=menus-default.ui
  os_style_css=

  OS_DEPENDENT_RESOURCE_FILES='<file compressed="true">gtk/menus-traditional.ui</file>'
fi

if test "x$os_style_css" != "x"; then
  OS_DEPENDENT_RESOURCE_FILES+="<file>css/emu-style-os.css</file>"
  AC_CONFIG_LINKS(src/resources/css/emu-style-os.css:src/resources/css/$os_style_css)
fi

AC_SUBST(OS_DEPENDENT_RESOURCE_FILES)
AC_CONFIG_LINKS(src/resources/gtk/menus.ui:src/resources/gtk/$os_menus_ui)

AC_CONFIG_FILES([
  Makefile
  data/org.colinkinloch.emulatron.gschema.xml
  src/resources/emu.gresource.xml
  po/Makefile.in
])
#	data/Makefile
AC_OUTPUT
