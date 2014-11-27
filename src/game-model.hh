#pragma once

#include <gtkmm/treemodel.h>
#include <gdkmm/pixbuf.h>

class GameModel:
    public Gtk::TreeModel
{
public:
  class ColumnRecord:
    public Gtk::TreeModel::ColumnRecord
  {
  public:
    Gtk::TreeModelColumn<Glib::ustring> filename;
    Gtk::TreeModelColumn<Glib::ustring> md5;
    Gtk::TreeModelColumn<Glib::ustring> title;
    Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf>> cover;
    Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf>> thumbnail;
    ColumnRecord() { add(filename); add(md5); add(title); add(cover); add(thumbnail); }
  };
};
