data_desktopdir = $(datadir)/applications
data_desktop_in_files = data/org.colinkinloch.emulatron.desktop.include
data_desktop_DATA = $(data_desktop_in_files):.desktop.in=.desktop

data_appdatadir = $(datadir)/appdata
data_appdata_in_files = data/org.colinkinloch.emulatron.appdata.xml.in
data_appdata_DATA = $(data_appdata_in_files:.xml.in=.xml)
@INTLTOOL_XML_RULE@

EXTRA_DIST += 					\
	$(data_desktop_in_files)		\
	$(data_appdata_in_files)

CLEANFILES +=	 			\
	$(data_desktop_DATA)		\
	$(data_appdata_DATA)

DISTCLEANFILES +=	 		\
	$(data_desktop_DATA)		\
