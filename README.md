Emulatron
=========
A GTK+ frontend for libRetro emulators.

Inspired by [OpenEmu](https://github.com/OpenEmu/OpenEmu).

Dependencies
------------
* pulseaudio
* gtkmm
* libgdamm
* SDL2

Screenshots
-----------
![sfii](https://raw.githubusercontent.com/wiki/ColinKinloch/Emulatron/sfii.png "Street Fighter II")
![mainwin](https://raw.githubusercontent.com/wiki/ColinKinloch/Emulatron/window.png "Main Window")
![controlpref](https://raw.githubusercontent.com/wiki/ColinKinloch/Emulatron/controls.png "Control Preferences")

Build
-----
###Fedora###
```bash
yum groupinstall "Development Tools"
yum install autoconf automake gcc-c++ autoconf-archive
yum install pulseaudio-libs-devel gtkmm30-devel libgda-sqlite libgdamm-devel SDL2-devel gobject-introspection-devel
autoreconf --install
./configure
make
sudo cp data/org.colinkinloch.emulatron.gschema.xml /usr/share/glib-2.0/schemas
sudo glib-compile-schemas /usr/share/glib-2.0/schemas
./src/emulatron
```

(c) Colin Kinloch - colin@kinlo.ch - 2014
