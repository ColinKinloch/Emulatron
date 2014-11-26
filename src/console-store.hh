#pragma once

#include "console-model.hh"

#include <gtkmm/treestore.h>
#include <gtkmm/liststore.h>
#include <libgdamm.h>

class ConsoleStore:
  public Gtk::ListStore
{
public:
  
  static Glib::RefPtr<ConsoleStore> create(const ConsoleModel::ColumnRecord& columns);
  static Glib::RefPtr<ConsoleStore> create();
  
  ConsoleModel::ColumnRecord col;
  
protected:
  ConsoleStore();
  ConsoleStore(const ConsoleModel::ColumnRecord& columns);
  virtual ~ConsoleStore();
  
  Glib::RefPtr<Gnome::Gda::Connection> openVGDB;
};
