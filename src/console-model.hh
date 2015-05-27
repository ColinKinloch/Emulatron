#pragma once

#include <gtkmm/treemodel.h>
#include <gdkmm/pixbuf.h>

class ConsoleModel:
  public Gtk::TreeModel
{
public:
  class ColumnRecord:
    public Gtk::TreeModel::ColumnRecord
  {
  public:
    Gtk::TreeModelColumn<int> id;
    Gtk::TreeModelColumn<Glib::ustring> name;
    Gtk::TreeModelColumn<Glib::ustring> shortName;
    Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf> > icon;
    ColumnRecord() { add(id); add(name); add(shortName); add(icon); }
  };
};
