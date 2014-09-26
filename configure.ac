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

AM_INIT_AUTOMAKE([foreign -Wall -Werror])

GETTEXT_PACKAGE=emulatron
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, ["$GETTEXT_PACKAGE"], [Define to the Gettext package name])
AC_SUBST(GETTEXT_PACKAGE)
#IT_PROG_INTLTOOL([0.40.0])

GOBJECT_INTROSPECTION_CHECK([0.6.7])
m4_define([glib_required_version], [2.23.0])
AM_PATH_GLIB_2_0(glib_required_version)

AC_PROG_CC
AC_LANG([C++])
AC_PROG_CXX
AX_CXX_COMPILE_STDCXX_11


# Initialize libtool
LT_PREREQ([2.2])
LT_INIT
m4_ifdef([LT_OUTPUT], [LT_OUTPUT])

PKG_PROG_PKG_CONFIG

AC_PATH_PROG([GLIB_MKENUMS],[glib-mkenums])

# Requirements
GLIB_REQS=2.35.0
GIO_REQS=2.27.92
GTK_REQS=3.11.5

# Used in the pkg-config file for plugins
AC_SUBST(GLIB_REQS)
AC_SUBST(GTK_REQS)

EMULATRON_VERSION_MAJOR=emulatron_version_major
EMULATRON_VERSION_MINOR=emulatron_version_minor
EMULATRON_VERSION_MICRO=emulatron_version_micro
AC_SUBST(EMULATRON_VERSION_MAJOR)
AC_SUBST(EMULATRON_VERSION_MINOR)
AC_SUBST(EMULATRON_VERSION_MICRO)


#LIBRETRO_CORES=""

#PKG_CHECK_MODULES(LIBRETRO, $LIBRETRO_CORES)

PKG_CHECK_MODULES(PLAYER, gtkmm-3.0 glib-2.0 >= $GLIB_REQS gio-2.0 >= $GIO_REQS gtk+-3.0 >= $GTK_REQS gdk-x11-3.0 >= $GTK_REQS gmodule-2.0 gnome-desktop-3.0)

AC_CONFIG_FILES([
  Makefile
  src/Makefile
])
AC_OUTPUT
