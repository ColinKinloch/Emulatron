#pragma once

#include <gtkmm/builder.h>

#include "emu-window.hh"

class Emulatron: public Gtk::Application
{
protected:
  EmuWindow* emuWindow;
  Glib::RefPtr<Gtk::Builder> refBuilder;
  Glib::RefPtr<Gtk::Settings> settings;
  
public:
  Emulatron(int& argc, char**& argv, const Glib::ustring& application_id=Glib::ustring(), Gio::ApplicationFlags flags=Gio::APPLICATION_FLAGS_NONE);
};
