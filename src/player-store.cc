#include "player-store.hh"
#include <iostream>
#include <glibmm/stringutils.h>

PlayerStore::PlayerStore(const PlayerModel::ColumnRecord& columns, int count):
  Gtk::TreeStore()
{
  set_column_types(columns);
  
  for(int i=1; i<=count; ++i)
  {
    Gtk::TreeModel::Row row = *(append());
    row[col.number] = i;
    row[col.name] = "Player "+Glib::Ascii::dtostr(i);
  }
}
PlayerStore::PlayerStore(int count):
  PlayerStore::PlayerStore(col, count)
{}
PlayerStore::~PlayerStore()
{}

Glib::RefPtr<PlayerStore> PlayerStore::create(const PlayerModel::ColumnRecord& columns, int count)
{
  return Glib::RefPtr<PlayerStore>(new PlayerStore(columns, count));
}
Glib::RefPtr<PlayerStore> PlayerStore::create(int count)
{
  return Glib::RefPtr<PlayerStore>(new PlayerStore(count));
}
Glib::RefPtr<PlayerStore> PlayerStore::create()
{
  return Glib::RefPtr<PlayerStore>(new PlayerStore());
}
