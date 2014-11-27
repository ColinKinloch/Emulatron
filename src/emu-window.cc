#include "emu-window.hh"
#include <iostream>
#include <libgdamm.h>
using namespace Gnome;

EmuWindow::EmuWindow(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refBuilder)
  : Gtk::ApplicationWindow(cobject)
{
  set_icon(Gdk::Pixbuf::create_from_resource("/org/colinkinloch/emulatron/img/joy-angle-256.png"));
  
  Gda::init();
  
  consoleList = ConsoleStore::create();
  
  /*
  Gtk::TreeModel::Row consoleRow = *(consoleList->append());
  consoleRow[consoleCols.name] = "Super Nintendo (SNES)";
    consoleRow[consoleCols.icon] = Gdk::Pixbuf::create_from_resource("/org/colinkinloch/emulatron/systems/nes/famicom_library.png");
  consoleRow = *(consoleList->append());
  consoleRow[consoleCols.name] = "MAME";
  consoleRow[consoleCols.icon] = Gdk::Pixbuf::create_from_resource("/org/colinkinloch/emulatron/img/joy-angle-256.png", 16, 16);
  consoleRow = *(consoleList->append());
  consoleRow[consoleCols.name] = "Nintendo 64 (NES)";
  consoleRow[consoleCols.icon] = Gdk::Pixbuf::create_from_resource("/org/colinkinloch/emulatron/img/controller_n64.png", 16, 16);
  */
  
  collectionList = CollectionStore::create();
  collectionList->add("MegaMan");
  
  refBuilder->get_widget("collection_selector", collectionSelector);
  refBuilder->get_widget("console_selector", consoleSelector);
  refBuilder->get_widget_derived("game_icon_view", gameIconView);
  refBuilder->get_widget("game_tree_view", gameTreeView);
  
  gameTreeView->set_model(gameIconView->get_model());
  
  consoleSelector->set_model(consoleList);
  collectionSelector->set_model(collectionList);
  
  //collectionSelector->set_entry_text_column(consoleList->col.name);
  
  Gtk::ToggleButton* viewGridButton;
  Gtk::ToggleButton* viewListButton;
  
  refBuilder->get_widget("view_grid_button", viewGridButton);
  refBuilder->get_widget("view_list_button", viewListButton);
  
  
  auto view = Gio::SimpleActionGroup::create();
  
  //auto layout = Gio::SimpleAction::create_radio_string("layout", "grid");
  //layout->signal_change_state().connect(sigc::mem_fun(*this, &EmuWindow::on_view_change));
  
  view->add_action_radio_string("layout",
    sigc::mem_fun(*this, &EmuWindow::on_layout_change),
    "grid");
  
  insert_action_group("view", view);
  //view->insert(layout);
  //viewGridButton->insert_action_group(view);
  //viewListButton->set_related_action(view);
  
  refBuilder->get_widget("search_bar", searchBar);
  refBuilder->get_widget("search_button", searchButton);
  
  //searchButton->signal_toggled().connect(sigc::mem_fun(this, &EmuWindow::on_game_search_mode));
  
  //searchButton->property_active().signal_changed().connect(sigc::mem_fun(this, &EmuWindow::on_game_search_mode));
  //searchButton->property_active().signal_changed().connect(sigc::bind(m_signal_search_mode, true));
  searchButton->connect_property_changed("active", sigc::mem_fun(this, &EmuWindow::on_game_search_mode));
  
  //signal_search_mode().connect(sigc::mem_fun(this, &EmuWindow::on_search_mode));
  
  searchBar->property_search_mode_enabled().signal_changed().connect(sigc::mem_fun(this, &EmuWindow::on_game_search_mode));
}
EmuWindow::~EmuWindow()
{
  
}

EmuWindow::type_signal_search_mode EmuWindow::signal_search_mode()
{
  return m_signal_search_mode;
}

void EmuWindow::on_layout_change(const Glib::ustring& value)
{
  std::cout<<"Layout changed to "<<value<<std::endl;
}
void EmuWindow::on_change_collection(const Gtk::TreeModel::Path& path)
{
  auto iter = consoleList->get_iter(path);
  if(iter)
  {
    Gtk::TreeModel::Row row = *iter;
  }
}
void EmuWindow::on_game_search_mode()
{
  std::cout<<"Doing"<<std::endl;
  m_signal_search_mode.emit(searchButton->get_active());
}
void EmuWindow::on_search_mode(bool value)
{
  std::cout<<"Doing"<<std::endl;
  searchBar->set_search_mode(value);
  std::cout<<"Search mode ";
  if(value)
   std::cout<<"on";
  else
   std::cout<<"off";
  std::cout<<std::endl;
}

