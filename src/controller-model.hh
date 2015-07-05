#pragma once

#include "controller.hh"
#include <gtkmm/treemodel.h>
#include <SDL.h>

class ControllerModel:
  public Gtk::TreeModel
{
public:
  class ColumnRecord:
    public Gtk::TreeModel::ColumnRecord
  {
  public:
    Gtk::TreeModelColumn<Controller*> cont;
    Gtk::TreeModelColumn<Glib::ustring> name;
    Gtk::TreeModelColumn<bool> attached;
    ColumnRecord() { add(cont); add(name); add(attached);};
  };
};
