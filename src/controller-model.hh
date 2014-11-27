#pragma once

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
    Gtk::TreeModelColumn<SDL_GameController*> sdl;
    Gtk::TreeModelColumn<Glib::ustring> name;
    Gtk::TreeModelColumn<bool> attached;
    ColumnRecord() { add(sdl); add(name); add(attached);};
  };
};
