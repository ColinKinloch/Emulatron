#include "emu-window.hh"
#include <iostream>

EmuWindow::EmuWindow()
{
  
}
EmuWindow::EmuWindow(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refBuilder)
  : Gtk::ApplicationWindow(cobject)
{
  set_icon(Gdk::Pixbuf::create_from_resource("/org/colinkinloch/emulatron/img/joy-angle-256.png"));
}
EmuWindow::~EmuWindow()
{
  
}
