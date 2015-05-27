#pragma once

#include <gtkmm/treemodel.h>
#include <gdkmm/pixbuf.h>

class CollectionModel:
  public Gtk::TreeModel
{
public:
  class ColumnRecord:
    public Gtk::TreeModel::ColumnRecord
  {
  public:
    Gtk::TreeModelColumn<Glib::ustring> name;
    Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf> > icon;
    ColumnRecord() { add(name); add(icon); };
  };
};
