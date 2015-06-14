SUBDIRS = #src #data

EXTRA_DIST = \
	BUGS \
	HACKING \
	MAINTAINERS \
	emu.doap \
	intltool-extract.in \
	intltool-merge.in \
	intltool-update.in

DISTCLEANFILES = \
	gnome-doc-utils.make    \
	intltool-extract 	\
	intltool-merge 		\
	intltool-update

MAINTAINERCLEANFILES = \
	$(GITIGNORE_MAINTAINERCLEANFILES_TOPLEVEL) \
	$(GITIGNORE_MAINTAINERCLEANFILES_MAKEFILE_IN) \
	$(GITIGNORE_MAINTAINERCLEANFILES_M4_LIBTOOL) \
	ABOUT-NLS		\
	INSTALL			\
	config.rpath		\
	gtk-doc.make		\
	py-compile		\
	test-driver		\
	m4/gnome-doc-utils.m4	\
	m4/gtk-doc.m4		\
	m4/intltool.m4		\
	m4/codeset.m4		\
	m4/fcntl-o.m4		\
	m4/gettext.m4		\
	m4/glibc2.m4		\
	m4/glibc21.m4		\
	m4/iconv.m4		\
	m4/intdiv0.m4		\
	m4/intl.m4		\
	m4/intldir.m4		\
	m4/intlmacosx.m4	\
	m4/intmax.m4		\
	m4/inttypes-pri.m4	\
	m4/inttypes_h.m4	\
	m4/lcmessage.m4		\
	m4/lib-ld.m4		\
	m4/lib-link.m4		\
	m4/lib-prefix.m4	\
	m4/lock.m4		\
	m4/longlong.m4		\
	m4/nls.m4		\
	m4/po.m4		\
	m4/printf-posix.m4	\
	m4/progtest.m4		\
	m4/size_max.m4		\
	m4/stdint_h.m4		\
	m4/threadlib.m4		\
	m4/uintmax_t.m4		\
	m4/visibility.m4	\
	m4/wchar_t.m4		\
	m4/wint_t.m4		\
	m4/xsize.m4		\
	po/Makevars.template	\
	po/Rules-quot		\
	po/*.sed		\
	po/*.header		\
	po/*.sin

DISTCHECK_CONFIGURE_FLAGS = --enable-gtk-doc

bin_PROGRAMS =
gsettings_SCHEMAS =
BUILT_SOURCES = 
CLEANFILES = emulator.spec

include data/Makefile.am
include src/Makefile.am

#GSETTINGS_RULES@

CLEANFILES += emulator.spec


# Build ChangeLog from GIT history
ChangeLog:
	@if test -f $(top_srcdir)/.git/HEAD; then \
		git log --stat --after="Tue Sep 23 18:06:04 2014" > $@; \
	fi

CLEANFILES += $(gsettings_SCHEMAS) $(gsettings_ENUMS) $(BUILT_SOURCES)
DISTCLEANFILES += $(gsettings_SCHEMAS) $(gsettings_ENUMS) $(BUILT_SOURCES)
MAINTAINERCLEANFILES += $(gsettings_SCHEMAS:.xml=.valid)

#INTLTOOL_DESKTOP_RULE@
#INTLTOOL_XML_RULE@

dist-hook: dist-hook-cleanup

dist-hook-cleanup:
	cd $(distdir); rm -f $(BUILT_SOURCES)

.PHONY: dist-hook-cleanup
