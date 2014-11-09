
#include "config.h"

#include <iostream>

#include <glib-object.h>
#include <glib/gi18n.h>
#include <gtkmm.h>
#include <gdkmm/pixbuf.h>

#include "emulatron.hh"
#include "emu-window.hh"
#include "emu-about-dialog.hh"
#include "emu-preference-window.hh"
#include "game-store.h"

#include "emu-resources.h"

EmuWindow* mainWindow = nullptr;
EmuPreferenceWindow* prefWindow = nullptr;
EmuAboutDialog* aboutDialog = nullptr;
//Glib::RefPtr<Gio::MenuModel> appMenu;

Gtk::IconView* gameSelector = nullptr;
Gtk::TreeView* consoleSelector = nullptr;
Gtk::TreeView* collectionSelector = nullptr;

GameStore::ColumnRecord gameCols;
ConsoleStore::ColumnRecord consoleCols;
Glib::RefPtr<Gtk::TreeStore> gameList;
Glib::RefPtr<Gtk::TreeStore> consoleList;
Glib::RefPtr<Gtk::TreeStore> collectionList;


Emulatron::Emulatron(int& argc, char**& argv, const Glib::ustring& application_id, Gio::ApplicationFlags flags): Gtk::Application(argc, argv, application_id, flags)
{
  refBuilder = Gtk::Builder::create();
  refBuilder->set_application(Glib::RefPtr<Emulatron>(this));
  
  settings = Gtk::Settings::get_default();
  settings->property_gtk_application_prefer_dark_theme() = true;
  
  try
  {
    refBuilder->add_from_resource("/org/colinkinloch/emulatron/gtk/menus.ui");
    refBuilder->add_from_resource("/org/colinkinloch/emulatron/ui/emu-view-actions.ui");
    refBuilder->add_from_resource("/org/colinkinloch/emulatron/ui/emu-preference-dialog.ui");
    refBuilder->add_from_resource("/org/colinkinloch/emulatron/ui/emu-main-window.ui");
  }
  catch(const Gio::ResourceError& ex)
  {
    std::cerr<<"ResourceError: "<< ex.what() <<std::endl;
  }
  catch(const Glib::MarkupError& ex)
  {
    std::cerr<<"MarkupError: "<< ex.what() <<std::endl;
  }
  catch(const Gtk::BuilderError& ex)
  {
    std::cerr << "BuilderError: " << ex.what() << std::endl;
  }
  
  try
  {
    refBuilder->get_widget_derived("emu_main_window", mainWindow);
  }
  catch(const Gtk::BuilderError& ex)
  {
    std::cerr << "BuilderError: " << ex.what() << std::endl;
  }
}

int main(int argc, char **argv)
{
  Glib::RefPtr<Emulatron> app = Glib::RefPtr<Emulatron>(new Emulatron(argc, argv, "org.colinkinloch.emulatron"));
  
  Glib::RefPtr<Gtk::Builder> refBuilder = Gtk::Builder::create();
  refBuilder->set_application(app);
  
  Glib::RefPtr<Gtk::Settings> settings = Gtk::Settings::get_default();
  settings->property_gtk_application_prefer_dark_theme() = true;

  gameList = GameStore::create(gameCols);
  
  Gtk::TreeModel::Row gameRow = *(gameList->append());
  gameRow[gameCols.title] = "The Legend of Zelda: A Link to the Past";
  gameRow[gameCols.filename] = "/dev/null";
  gameRow[gameCols.thumbnail] = Gdk::Pixbuf::create_from_resource("/org/colinkinloch/emulatron/img/lttp.jpg", 200, 200);
  gameRow = *(gameList->append());
  gameRow[gameCols.title] = "Other game";
  gameRow[gameCols.filename] = "/dev/null";
  gameRow[gameCols.thumbnail] = Gdk::Pixbuf::create_from_resource("/org/colinkinloch/emulatron/img/dino_down.png", 200, 200);
  gameRow = *(gameList->append());
  gameRow[gameCols.title] = "Game number 3";
  gameRow[gameCols.filename] = "/dev/null";
  gameRow[gameCols.thumbnail] = Gdk::Pixbuf::create_from_resource("/org/colinkinloch/emulatron/img/logo.png", 200, 200);
  gameRow = *(gameList->append());
  gameRow[gameCols.title] = "Silly big game";
  gameRow[gameCols.filename] = "/dev/null";
  gameRow[gameCols.thumbnail] = Gdk::Pixbuf::create_from_resource("/org/colinkinloch/emulatron/img/joy-angle-1024.png", 200, 200);
  
  consoleList = ConsoleStore::create(consoleCols);
  
  
  Gtk::TreeModel::Row consoleRow = *(consoleList->append());
  consoleRow[consoleCols.title] = "Super Nintendo (SNES)";
  consoleRow[consoleCols.icon] = Gdk::Pixbuf::create_from_resource("/org/colinkinloch/emulatron/img/dino_down.png")->scale_simple(16, 16, Gdk::INTERP_NEAREST);
  consoleRow = *(consoleList->append());
  consoleRow[consoleCols.title] = "Nintendo (NES)";
  consoleRow[consoleCols.icon] = Gdk::Pixbuf::create_from_resource("/org/colinkinloch/emulatron/img/dino_down.png")->scale_simple(16, 16, Gdk::INTERP_NEAREST);
  
  collectionList = ConsoleStore::create(consoleCols);
  
  Gtk::TreeModel::Row collectionRow = *(collectionList->append());
  collectionRow[consoleCols.title] = "All Games";
  collectionRow[consoleCols.icon] = Gdk::Pixbuf::create_from_resource("/org/colinkinloch/emulatron/img/dino_down.png")->scale_simple(16, 16, Gdk::INTERP_NEAREST);
  collectionRow = *(collectionList->append());
  collectionRow[consoleCols.title] = "Recently Added";
  collectionRow[consoleCols.icon] = Gdk::Pixbuf::create_from_resource("/org/colinkinloch/emulatron/img/dino_down.png")->scale_simple(16, 16, Gdk::INTERP_NEAREST);
  
  
  try
  {
    refBuilder->add_from_resource("/org/colinkinloch/emulatron/gtk/menus.ui");
    refBuilder->add_from_resource("/org/colinkinloch/emulatron/ui/emu-view-actions.ui");
    refBuilder->add_from_resource("/org/colinkinloch/emulatron/ui/emu-preference-dialog.ui");
    refBuilder->add_from_resource("/org/colinkinloch/emulatron/ui/emu-main-window.ui");
  }
  catch(const Gio::ResourceError& ex)
  {
    std::cerr<<"ResourceError: "<< ex.what() <<std::endl;
    return 1;
  }
  catch(const Glib::MarkupError& ex)
  {
    std::cerr<<"MarkupError: "<< ex.what() <<std::endl;
    return 1;
  }
  catch(const Gtk::BuilderError& ex)
  {
    std::cerr << "BuilderError: " << ex.what() << std::endl;
    return 1;
  }
  
  try
  {
    refBuilder->get_widget_derived("emu_main_window", mainWindow);
    refBuilder->get_widget_derived("emu_pref_window", prefWindow);
    refBuilder->get_widget_derived("emu_about_dialog", aboutDialog);
    refBuilder->get_widget("game_selector", gameSelector);
    refBuilder->get_widget("console_selector", consoleSelector);
    refBuilder->get_widget("collection_selector", collectionSelector);
  }
  catch(const Gtk::BuilderError& ex)
  {
    std::cerr << "BuilderError: " << ex.what() << std::endl;
  }
  
  gameSelector->set_model(gameList);
  gameSelector->set_text_column(gameCols.title);
  gameSelector->set_pixbuf_column(gameCols.thumbnail);
  
  consoleSelector->set_model(consoleList);
  collectionSelector->set_model(collectionList);
  
  //appMenu = refBuilder->get_object("app-menu");
  if(mainWindow)
  {
    
    prefWindow->show();
    
    aboutDialog->set_transient_for(*mainWindow);
    aboutDialog->show();
    
    //mainWindow->set_icon(Gdk::Pixbuf::create_from_resource("/org/colinkinloch/emulatron/img/joy-angle-256.png"));
    
    app->get_menu_by_id("app-menu");
    
    app->add_action("quit");
    
    //app->set_app_menu(*appMenu);
    //app->set_menubar(refBuilder->get_menu_by_id("app-menu"));

    app->run(*mainWindow, argc, argv);
  }
  
  return 0;
}
