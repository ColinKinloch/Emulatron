#pragma once

#include <string>
#include <unordered_map>

#include <pulse/pulseaudio.h>
#include <gtkmm/builder.h>

#include "emu-window.hh"
#include "emu-about-dialog.hh"
#include "emu-preference-window.hh"

#include "retro/console.hh"
#include "audio.hh"

class Emulatron: public Gtk::Application
{
protected:
  Glib::RefPtr<Gtk::Builder> refBuilder;
  Glib::RefPtr<Gtk::Settings> uiSettings;

  std::unordered_map<std::string, std::string> consoles;

  void view_game_area();
  void view_gl();
  bool draw_cairo(const Cairo::RefPtr<Cairo::Context>& cr);
  void resize_cairo(Gtk::Allocation a);

  void vf(const void *d, unsigned w, unsigned h, size_t p);
  void as(int16_t left, int16_t right);
  size_t asb(const int16_t *data, size_t frames);
  void ip();
  int16_t is(unsigned port, unsigned device, unsigned index, unsigned id);

  void setVolume(double vol);

  void on_open();

  void on_preferences();
  
  void on_about();
  void on_quit();
  
  void startGame(const Gtk::TreeModel::Path& path);
  void stepGame();

  void trigger_draw();
  void trigger_audio();
  void trigger_input_poll();

  bool running;

  Glib::RefPtr<Glib::TimeoutSource> retroClock;

public:

  EmuWindow* emuWindow;
  EmuPreferenceWindow* prefWindow;
  EmuAboutDialog* aboutDialog;
  Gtk::GLArea* gameArea;
  Gtk::Image* gameImageArea;
  Gtk::DrawingArea* gameCairoArea;
  Gtk::Stack* emuMainStack;
  Gtk::Stack* emuGameStack;
  Gtk::VolumeButton* volumeSlider;
  Gtk::ToggleButton* pauseButton;
  Gtk::Button* resetButton;

  sigc::connection vidCon;
  sigc::connection inpCon;
  sigc::connection playCon;
  sigc::connection resetCon;

  Gtk::Allocation alloc;
  Cairo::RefPtr<Cairo::ImageSurface> surf;
  Cairo::Matrix mat;

  Glib::RefPtr<Gio::Settings> settings;

  Retro::Core* core;
  Retro::Console* console;
  Audio* audio;

  Emulatron(int& argc, char**& argv);
};
