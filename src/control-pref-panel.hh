#pragma once

#include <gtkmm/box.h>
#include <gtkmm/builder.h>

#include <gtkmm/image.h>
#include <gtkmm/combobox.h>
#include <gtkmm/treestore.h>
#include <gtkmm/liststore.h>

#include "console-store.hh"
#include "controller-store.hh"
#include "player-store.hh"

class ControlPrefPanel:
  public Gtk::Box
{
public:
  ControlPrefPanel();
  ControlPrefPanel(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refBuilder);
  ~ControlPrefPanel();
protected:
  Gtk::Image* image;
  
  Glib::RefPtr<ConsoleStore> consoles;
  Glib::RefPtr<ControllerStore> controllers;
  Glib::RefPtr<PlayerStore> players;
  
  Gtk::ComboBox* consoleCombo;
  Gtk::ComboBox* playerCombo;
  Gtk::ComboBox* controllerCombo;
  
  void on_console_changed();
};
