
#pragma once

#include <gtkmm.h>

class GameStore:
    public Gtk::TreeStore
{
public:
  class ColumnRecord: public Gtk::TreeModel::ColumnRecord
  {
  public:
    Gtk::TreeModelColumn<Glib::ustring> title;
    Gtk::TreeModelColumn<Glib::ustring> filename;
    Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf>> thumbnail;
    ColumnRecord() { add(title); add(filename); add(thumbnail); }
  };
  static Glib::RefPtr<GameStore> create(const GameStore::ColumnRecord& columns);
  
protected:
  GameStore(const GameStore::ColumnRecord& columns);
  virtual ~GameStore();
  
  
private:
  
};

class EmulatorStore:
  public Gtk::TreeStore
{
public:
  class ColumnRecord: public Gtk::TreeModel::ColumnRecord
  {
  public:
    Gtk::TreeModelColumn<Glib::ustring> title;
    Gtk::TreeModelColumn<Glib::ustring> path;
    Gtk::TreeModelColumn<bool> gl;
    ColumnRecord() { add(title); add(path); add(gl); }
  };
};

class ConsoleStore:
  public Gtk::TreeStore
{
public:
  class ColumnRecord: public Gtk::TreeModel::ColumnRecord
  {
  public:
    Gtk::TreeModelColumn<Glib::ustring> title;
    Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf>> icon;
    Gtk::TreeModelColumn<Glib::RefPtr<GameStore>> games;
    ColumnRecord() { add(title); add(icon); }
  };
};
