#pragma once

#include <gtkmm/window.h>
#include <gtkmm/box.h>
#include <gtkmm/builder.h>

#include "control-pref-panel.hh"

class EmuPreferenceWindow:
  public Gtk::Window
{
public:
  EmuPreferenceWindow();
  EmuPreferenceWindow(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refBuilder);
  virtual ~EmuPreferenceWindow();
  
protected:
  
  Gtk::Box* libraryPanel;
  Gtk::Box* gameplayPanel;
  ControlPrefPanel* controlPanel;
  Gtk::Box* corePanel;
};
