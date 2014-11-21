#include "game-store.hh"

GameStore::GameStore(const GameStore::ColumnRecord& columns): Gtk::TreeStore()
{
  set_column_types(columns);
}
GameStore::~GameStore()
{
  
}

Glib::RefPtr<GameStore> GameStore::create(const GameStore::ColumnRecord& columns)
{
  return Glib::RefPtr<GameStore>(new GameStore(columns));
}
