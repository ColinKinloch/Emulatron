#pragma once

#include <gtkmm/treestore.h>

#include "player-model.hh"

class PlayerStore:
  public Gtk::TreeStore
{
public:
  
  static Glib::RefPtr<PlayerStore> create(const PlayerModel::ColumnRecord& columns, int count=0);
  static Glib::RefPtr<PlayerStore> create(int count=0);
  static Glib::RefPtr<PlayerStore> create();
  
  PlayerModel::ColumnRecord col;
  
protected:
  PlayerStore(const PlayerModel::ColumnRecord& columns, int count=0);
  PlayerStore(int count=0);
  virtual ~PlayerStore();
};
