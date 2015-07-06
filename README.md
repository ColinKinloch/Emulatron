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
![sfii](https://raw.githubusercontent.com/wiki/ColinKinloch/Emulatron/sfii.png "Street Fighter II")
![mainwin](https://raw.githubusercontent.com/wiki/ColinKinloch/Emulatron/window.png "Main Window")
![controlpref](https://raw.githubusercontent.com/wiki/ColinKinloch/Emulatron/controls.png "Control Preferences")

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
