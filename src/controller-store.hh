#pragma once

#include "controller-model.hh"

#include <gtkmm/treestore.h>
#include <gtkmm/liststore.h>

class ControllerStore:
  public Gtk::TreeStore
{
public:
  
  static Glib::RefPtr<ControllerStore> create(const ControllerModel::ColumnRecord& columns);
  static Glib::RefPtr<ControllerStore> create();
  
  ControllerModel::ColumnRecord col;
  
  void update();
  void update(ControllerModel::iterator iter);
  void pollAll();
  
protected:
  ControllerStore();
  ControllerStore(const ControllerModel::ColumnRecord& columns);
  virtual ~ControllerStore();
};
