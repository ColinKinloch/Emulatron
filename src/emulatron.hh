#pragma once

#include <gtkmm/builder.h>

#include "emu-window.hh"

class Emulatron: public Gtk::Application
{
protected:
  Glib::RefPtr<Gtk::Builder> refBuilder;
  Glib::RefPtr<Gtk::Settings> settings;
  
  void on_preferences();
  
  void on_about();
  void on_quit();
  
public:
  EmuWindow* emuWindow;
  EmuPreferenceWindow* prefWindow;
  EmuAboutDialog* aboutDialog;
  
  Emulatron(int& argc, char**& argv);
};
