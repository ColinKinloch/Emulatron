
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
  
  GameModel::ColumnRecord col;
protected:
  GameStore();
  GameStore(const GameModel::ColumnRecord& columns);
  virtual ~GameStore();
  
  Glib::RefPtr<Gnome::Gda::Connection> openVGDB;
  Glib::RefPtr<Gnome::Gda::SqlBuilder> builder;
  
  bool on_spin(GameModel::Row row);

  void on_file_loaded(const Glib::RefPtr<Gio::AsyncResult>& res, GameModel::Row row, Glib::RefPtr<Gio::File> file, sigc::connection* spin);
  void on_checksum_calculated(GameModel::Row row, Glib::RefPtr<Gio::File> file, sigc::connection* spin);
  void doChecksum(char* contents, gsize size, GameModel::Row row, Glib::RefPtr<Gio::File> file, sigc::connection* spin);
  void on_rom_data();
  void on_image_ready(const Glib::RefPtr<Gio::AsyncResult>& res, GameModel::Row row, Glib::RefPtr<Gio::File> file, sigc::connection* spin);
  
private:
  
  //Glib::RefPtr<Glib::TimeoutSource> spinClock;

  Glib::Dispatcher checksumDone;

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

