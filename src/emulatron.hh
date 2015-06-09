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

  Glib::RefPtr<Gtk::Builder> refBuilder;
  Glib::RefPtr<Gtk::Settings> settings;

  void view_gl();

  void on_open();

  void on_preferences();
  
  void on_about();
  void on_quit();
  
  void startGame(const Gtk::TreeModel::Path& path);
  bool stepGame();
  bool stepSound();


  Glib::RefPtr<Glib::TimeoutSource> retroClock;

public:
  EmuWindow* emuWindow;
  EmuPreferenceWindow* prefWindow;
  EmuAboutDialog* aboutDialog;
  Gtk::Widget* gameArea;
  Gtk::Image* gameImageArea;
  Gtk::Stack* emuMainStack;
  
  LibRetroCore* core;

  Emulatron(int& argc, char**& argv);
};
