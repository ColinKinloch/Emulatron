#pragma once

#include <gtkmm.h>

#include "game-store.hh"

class GameView:
    public Gtk::IconView
{
public:
  GameView(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refBuilder);
  ~GameView();
protected:
  Glib::RefPtr<GameStore> store;
  
  std::vector<Gtk::TargetEntry> targets;
  
  void on_item_activated(const Gtk::TreeModel::Path& path);
  bool on_drag_motion(const Glib::RefPtr<Gdk::DragContext>& context, int x, int y, guint time);
  bool on_drag_drop(const Glib::RefPtr<Gdk::DragContext>& context, int x, int y, guint time);
  void on_drag_data_received(const Glib::RefPtr<Gdk::DragContext>& context, int x, int y, const Gtk::SelectionData& selection_data, guint info, guint time);
};
