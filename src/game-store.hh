
#pragma once

#include "game-model.hh"

#include <gtkmm.h>
#include <libgdamm.h>

class GameStore:
    public Gtk::TreeStore
{
public:
  
  static Glib::RefPtr<GameStore> create(const GameModel::ColumnRecord& columns);
  static Glib::RefPtr<GameStore> create();
  
  bool add(std::string uri);
  bool add(Glib::RefPtr<Gio::File> file);
  
  GameModel::ColumnRecord col;
protected:
  GameStore();
  GameStore(const GameModel::ColumnRecord& columns);
  virtual ~GameStore();
  
  Glib::RefPtr<Gnome::Gda::Connection> openVGDB;
  Glib::RefPtr<Gnome::Gda::SqlBuilder> builder;
  
  void gatherROMData(GameModel::Row, Glib::RefPtr<Gio::File>, sigc::connection);

  bool onSpin(GameModel::Row row);

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

