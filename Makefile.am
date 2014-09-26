SUBDIRS = src

#EXTRA_DIST =      

#CLEANFILES = emulator.spec

# Build ChangeLog from GIT history
ChangeLog:
	@if test -f $(top_srcdir)/.git/HEAD; then \
		git log --stat --after="Tue Sep 23 18:06:04 2014" > $@; \
	fi
