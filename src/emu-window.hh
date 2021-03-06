#pragma once

#include <gtkmm/applicationwindow.h>
#include <gtkmm/builder.h>
#include <gtkmm/iconview.h>
#include <gtkmm/treeview.h>

#include <libgdamm.h>

#include "game-icon-view.hh"
#include "game-tree-view.hh"
#include "game-store.hh"
#include "console-store.hh"
#include "collection-store.hh"

class EmuWindow: public Gtk::ApplicationWindow
{
public:
  EmuWindow(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refBuilder);
  virtual ~EmuWindow();
  
  typedef sigc::signal<void, bool> type_signal_search_mode;
  type_signal_search_mode signal_search_mode();
  
  void on_layout_change(const Glib::ustring& value);
  
  GameIconView* gameIconView = nullptr;
  GameTreeView* gameTreeView = nullptr;

protected:
  Glib::RefPtr<Gnome::Gda::Connection> openVGDB;

  type_signal_search_mode m_signal_search_mode;
  
  Gtk::TreeView* consoleSelector = nullptr;
  Gtk::TreeView* collectionSelector = nullptr;
  
  Glib::RefPtr<ConsoleStore> consoleList;
  Glib::RefPtr<CollectionStore> collectionList;
  
  Gtk::SearchBar* searchBar;
  Gtk::ToggleButton* searchButton;
  
  void on_change_collection(const Gtk::TreeModel::Path& path);
  void on_game_activated(const Gtk::TreeModel::Path& path);
  void on_game_search_mode();
  void on_search_mode(bool value);
  bool on_game_drag_drop(const Glib::RefPtr<Gdk::DragContext>& context, int x, int y, guint time);
  bool on_game_drag_motion(const Glib::RefPtr<Gdk::DragContext>& context, int x, int y, guint time);
  void on_game_drag_data_received(const Glib::RefPtr<Gdk::DragContext>& context, int x, int y, const Gtk::SelectionData& selection_data, guint info, guint time);
};
