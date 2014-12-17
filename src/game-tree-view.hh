#pragma once

#include <gtkmm/treeview.h>

#include "game-store.hh"

class GameTreeView:
  public Gtk::TreeView
{
public:
  GameTreeView(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refBuilder);
  ~GameTreeView();
  
  void set_model(Glib::RefPtr<GameStore> model);
  Glib::RefPtr<GameStore> get_model();
protected:
  Glib::RefPtr<GameStore> store;
  
  std::vector<Gtk::TargetEntry> targets;
  
  void on_item_activated(const Gtk::TreeModel::Path& path);
  bool on_drag_motion(const Glib::RefPtr<Gdk::DragContext>& context, int x, int y, guint time);
  bool on_drag_drop(const Glib::RefPtr<Gdk::DragContext>& context, int x, int y, guint time);
  void on_drag_data_received(const Glib::RefPtr<Gdk::DragContext>& context, int x, int y, const Gtk::SelectionData& selection_data, guint info, guint time);
};
