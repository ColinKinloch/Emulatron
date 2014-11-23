#pragma once

#include <gtkmm.h>

class GameModel:
    public Gtk::TreeModel
{
public:
  class ColumnRecord: public Gtk::TreeModel::ColumnRecord
  {
  public:
    Gtk::TreeModelColumn<Glib::ustring> filename;
    Gtk::TreeModelColumn<Glib::ustring> md5;
    Gtk::TreeModelColumn<Glib::ustring> title;
    Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf>> cover;
    Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf>> thumbnail;
    ColumnRecord() { add(title); add(filename); add(cover); add(thumbnail); }
  };
  
  GameModel();
  ~GameModel();
};
