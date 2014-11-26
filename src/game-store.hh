
#pragma once

#include "game-model.hh"

#include <gtkmm.h>
#include <libgdamm.h>

class GameStore:
    public Gtk::TreeStore
{
public:
  
  bool add(std::string uri);
  
  static Glib::RefPtr<GameStore> create(const GameModel::ColumnRecord& columns);
  static Glib::RefPtr<GameStore> create();
  
  GameModel::ColumnRecord col;
  
protected:
  GameStore();
  GameStore(const GameModel::ColumnRecord& columns);
  virtual ~GameStore();
  
  Glib::RefPtr<Gnome::Gda::Connection> openVGDB;
  
  void on_rom_lookup(const Glib::RefPtr<Gio::AsyncResult>& res, GameModel::Row row, Glib::RefPtr<Gio::File> file);
  void on_image_ready(const Glib::RefPtr<Gio::AsyncResult>& res, GameModel::Row row, Glib::RefPtr<Gio::File> file);
  
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

