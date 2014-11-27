#include "collection-store.hh"
#include <iostream>

CollectionStore::CollectionStore(const CollectionModel::ColumnRecord& columns):
  Gtk::ListStore()
{
  set_column_types(columns);
  
  userIcon = Gdk::Pixbuf::create_from_resource("/org/colinkinloch/emulatron/img/collections-user.png");
  autoIcon = Gdk::Pixbuf::create_from_resource("/org/colinkinloch/emulatron/img/collections-auto.png");
  
  Gtk::TreeModel::Row row = *(append());
  row[col.name] = "All Games";
  row[col.icon] = autoIcon;
  row = *(append());
  row[col.name] = "Recently Added";
  row[col.icon] = autoIcon;
}
CollectionStore::CollectionStore():
  CollectionStore::CollectionStore(col)
{}
CollectionStore::~CollectionStore()
{}

Glib::RefPtr<CollectionStore> CollectionStore::create(const CollectionModel::ColumnRecord& columns)
{
  return Glib::RefPtr<CollectionStore>(new CollectionStore(columns));
}
Glib::RefPtr<CollectionStore> CollectionStore::create()
{
  return Glib::RefPtr<CollectionStore>(new CollectionStore());
}

CollectionModel::Row CollectionStore::add(Glib::ustring name)
{
  CollectionModel::Row row = *(append());
  row[col.name] = name;
  row[col.icon] = userIcon;
  return row;
}
