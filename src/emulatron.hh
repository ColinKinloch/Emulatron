#pragma once

#include <ao/ao.h>

#include <gtkmm/builder.h>

#include "emu-window.hh"
#include "emu-about-dialog.hh"
#include "emu-preference-window.hh"

#include "libretro-core.hh"

class Emulatron: public Gtk::Application
{
protected:
  ao_device *audioDev;
  ao_sample_format format;
  int defaultDriver;

  Glib::RefPtr<Gtk::Builder> refBuilder;
  Glib::RefPtr<Gtk::Settings> settings;
  
  void view_gl();

  void on_open();

  void on_preferences();
  
  void on_about();
  void on_quit();
  
public:
  EmuWindow* emuWindow;
  EmuPreferenceWindow* prefWindow;
  EmuAboutDialog* aboutDialog;
  Gtk::Widget* gameArea;
  Gtk::Stack* emuMainStack;
  
  LibRetroCore* core;

  Emulatron(int& argc, char**& argv);
};
