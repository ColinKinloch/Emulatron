Emulatron
=========
A GTK+ frontend for libRetro emulators.

So far just a UI plagiarising OpenEmu.

Dependencies
------------
* gtkmm
* libgdamm
* SDL2

Screenshots
-----------
![Main Window](https://raw.githubusercontent.com/wiki/ColinKinloch/Emulatron/window.png "Rom selection.")

Build
-----
###Fedora###
```bash
yum groupinstall "Development Tools"
yum install autoconf automake gcc-c++ autoconf-archive
yum install gtkmm30-devel libgda-sqlite libgdamm-devel SDL2-devel gobject-introspection-devel
autoreconf --install
./configure
make
./src/emulatron
```

(c) Colin Kinloch - colin@kinlo.ch - 2014
