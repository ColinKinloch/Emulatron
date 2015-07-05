#pragma once

#include <gtkmm/treemodel.h>

#include "controller.hh"
#include "controller-model.hh"

class PlayerModel:
  public Gtk::TreeModel
{
public:
  class ColumnRecord:
    public Gtk::TreeModel::ColumnRecord
  {
  public:
    Gtk::TreeModelColumn<int> number;
    Gtk::TreeModelColumn<Glib::ustring> name;
    Gtk::TreeModelColumn<ControllerModel::Row> controller;
    ColumnRecord() { add(number); add(name); add(controller); };
  };
};
