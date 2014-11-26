#pragma once

#include "collection-model.hh"

#include <gtkmm/treestore.h>
#include <gtkmm/liststore.h>

class CollectionStore:
  public Gtk::ListStore
{
public:
  
  static Glib::RefPtr<CollectionStore> create(const CollectionModel::ColumnRecord& columns);
  static Glib::RefPtr<CollectionStore> create();
  
  CollectionModel::ColumnRecord col;
  
  CollectionModel::Row add(Glib::ustring name);
  
protected:
  CollectionStore();
  CollectionStore(const CollectionModel::ColumnRecord& columns);
  virtual ~CollectionStore();
  
  Glib::RefPtr<Gdk::Pixbuf> autoIcon;
  Glib::RefPtr<Gdk::Pixbuf> userIcon;
  
};
