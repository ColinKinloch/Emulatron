#include "emu-window.hh"
#include <iostream>

EmuWindow::EmuWindow()
{
  
}
EmuWindow::EmuWindow(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refBuilder)
  : Gtk::ApplicationWindow(cobject)
{
  set_icon(Gdk::Pixbuf::create_from_resource("/org/colinkinloch/emulatron/img/joy-angle-256.png"));
  
  gameList = GameStore::create(gameCols);
  
  Gtk::TreeModel::Row gameRow = *(gameList->append());
  gameRow[gameCols.title] = "The Legend of Zelda: A Link to the Past";
  gameRow[gameCols.filename] = "/dev/null";
  gameRow[gameCols.thumbnail] = Gdk::Pixbuf::create_from_resource(
   "/org/colinkinloch/emulatron/img/lttp.jpg", 200, 200);
  gameRow = *(gameList->append());
  gameRow[gameCols.title] = "Other game";
  gameRow[gameCols.filename] = "/dev/null";
  gameRow[gameCols.thumbnail] = Gdk::Pixbuf::create_from_resource(
   "/org/colinkinloch/emulatron/img/dino_down.png", 200, 200);
  gameRow = *(gameList->append());
  gameRow[gameCols.title] = "Game number 3";
  gameRow[gameCols.filename] = "/dev/null";
  gameRow[gameCols.thumbnail] = Gdk::Pixbuf::create_from_resource(
   "/org/colinkinloch/emulatron/img/logo.png", 200, 200);
  gameRow = *(gameList->append());
  gameRow[gameCols.title] = "Silly big game";
  gameRow[gameCols.filename] = "/dev/null";
  gameRow[gameCols.thumbnail] = Gdk::Pixbuf::create_from_resource(
   "/org/colinkinloch/emulatron/img/joy-angle-1024.png", 200, 200);
  
  consoleList = ConsoleStore::create(consoleCols);
  
  
  Gtk::TreeModel::Row consoleRow = *(consoleList->append());
  consoleRow[consoleCols.title] = "Super Nintendo (SNES)";
  consoleRow[consoleCols.icon] = Gdk::Pixbuf::create_from_resource("/org/colinkinloch/emulatron/img/dino_down.png", 16, 16);
  consoleRow = *(consoleList->append());
  consoleRow[consoleCols.title] = "MAME";
  consoleRow[consoleCols.icon] = Gdk::Pixbuf::create_from_resource("/org/colinkinloch/emulatron/img/joy-angle-256.png", 16, 16);
  consoleRow = *(consoleList->append());
  consoleRow[consoleCols.title] = "Nintendo 64 (NES)";
  consoleRow[consoleCols.icon] = Gdk::Pixbuf::create_from_resource("/org/colinkinloch/emulatron/img/controller_n64.png", 16, 16);
  
  collectionList = ConsoleStore::create(consoleCols);
  
  Gtk::TreeModel::Row collectionRow = *(collectionList->append());
  collectionRow[consoleCols.title] = "All Games";
  collectionRow[consoleCols.icon] = Gdk::Pixbuf::create_from_resource("/org/colinkinloch/emulatron/img/dino_down.png")->scale_simple(16, 16, Gdk::INTERP_NEAREST);
  collectionRow = *(collectionList->append());
  collectionRow[consoleCols.title] = "Recently Added";
  collectionRow[consoleCols.icon] = Gdk::Pixbuf::create_from_resource("/org/colinkinloch/emulatron/img/dino_down.png")->scale_simple(16, 16, Gdk::INTERP_NEAREST);
  
  refBuilder->get_widget("collection_selector", collectionSelector);
  refBuilder->get_widget("console_selector", consoleSelector);
  refBuilder->get_widget("game_selector", gameSelector);
  
  gameSelector->set_model(gameList);
  gameSelector->set_text_column(gameCols.title);
  gameSelector->set_pixbuf_column(gameCols.thumbnail);
  
  gameSelector->signal_item_activated().connect(sigc::mem_fun(this, &EmuWindow::on_game_activated));
  
  consoleSelector->set_model(consoleList);
  collectionSelector->set_model(collectionList);
  
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
  
  std::vector<Gtk::TargetEntry> targs = {
    Gtk::TargetEntry("text/uri-list")
  };
  gameSelector->drag_dest_set(targs);
  gameSelector->drag_dest_add_uri_targets();
  //gameSelector->enable_model_drag_dest(targs, Gdk::ACTION_COPY);
  gameSelector->signal_drag_data_received()
   .connect(sigc::mem_fun(this, &EmuWindow::on_game_drag_data_received));
  gameSelector->signal_drag_motion()
   .connect(sigc::mem_fun(this, &EmuWindow::on_game_drag_motion));
  gameSelector->signal_drag_drop()
   .connect(sigc::mem_fun(this, &EmuWindow::on_game_drag_drop));
  
  auto actions = list_actions();
  for(auto it = actions.begin(); it != actions.end() ;++it)
   std::cout<<*it<<std::endl;
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
    std::cout<<row[consoleCols.title]<<" selected"<<std::endl;
  }
}
void EmuWindow::on_game_activated(const Gtk::TreeModel::Path& path)
{
  auto iter = gameList->get_iter(path);
  if(iter)
  {
    Gtk::TreeModel::Row row = *iter;
    std::cout<<"Begin game: "<<row[gameCols.filename]<<std::endl;
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
void EmuWindow::on_game_drag_data_received(const Glib::RefPtr<Gdk::DragContext>& context, int x, int y, const Gtk::SelectionData& selection_data, guint info, guint time)
{
  Gtk::TreeModel::Row gameRow;
  auto uris = selection_data.get_uris();
  for(auto it = uris.begin(); it != uris.end(); ++it)
  {
    std::cout<<"Data: "<<*it<<std::endl;
    gameRow = *(gameList->append());
    gameRow[gameCols.title] = *it;
    gameRow[gameCols.filename] = *it;
    //gameRow[gameCols.thumbnail] = Gdk::Pixbuf::create_from_resource(
     //"/org/colinkinloch/emulatron/img/lttp.jpg", 200, 200);
  }
}
bool EmuWindow::on_game_drag_drop(const Glib::RefPtr<Gdk::DragContext>& context, int x, int y, guint time)
{
  if(context->list_targets().size())
  {
    std::cout<<"Dropped"<<std::endl;
  }
  return false;
}
bool EmuWindow::on_game_drag_motion(const Glib::RefPtr<Gdk::DragContext>& context, int x, int y, guint time)
{
  //std::cout<<"motion"<<std::endl;
  return false;
}
