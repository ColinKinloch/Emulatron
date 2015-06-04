#pragma once

#include <pulse/pulseaudio.h>
#include <gtkmm/builder.h>

#include "emu-window.hh"
#include "emu-about-dialog.hh"
#include "emu-preference-window.hh"

#include "libretro-core.hh"

class Emulatron: public Gtk::Application
{
protected:

  pa_mainloop *pa_ml;
  pa_mainloop_api *pa_mlapi;
  pa_context *pa_ctx;
  pa_stream *playstream;

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
