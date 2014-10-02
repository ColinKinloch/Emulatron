
#include "config.h"

#include <iostream>
/*
#include <gtk/gtk.h>

int main(int argc, char **argv)
{
  GtkBuilder *builder;
  GtkWidget *window;
  
  gtk_init(&argc, &argv);
  
  builder = gtk_builder_new();
  gtk_builder_add_from_file(builder, "res/Emulator.ui", nullptr);
  
  window = GTK_WIDGET(gtk_builder_get_object(builder, "emulator_main_window"));
  gtk_builder_connect_signals(builder, NULL);
  g_object_unref(G_OBJECT(builder));
  
  gtk_widget_show(window);
  gtk_main();
  
  return 0;
}*/

#include <glib-object.h>
#include <glib/gi18n.h>
#include <gtkmm.h>
#include "emu-resources.h"

Gtk::ApplicationWindow* mainWindow = nullptr;
Gtk::Window* prefWindow = nullptr;
Gtk::Window* prefWindowTwo = nullptr;
Gtk::AboutDialog* aboutDialog = nullptr;
//Glib::RefPtr<Gio::MenuModel> appMenu;


int main(int argc, char **argv)
{
  Glib::RefPtr<Gtk::Application> app = Gtk::Application::create(argc, argv, "org.colinkinloch.emulator");
  
  Glib::RefPtr<Gtk::Builder> refBuilder = Gtk::Builder::create();
  refBuilder->set_application(app);
  
  Glib::RefPtr<Gtk::Settings> settings = Gtk::Settings::get_default();
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

  refBuilder->get_widget("emu_main_window", mainWindow);
  refBuilder->get_widget("emu_pref_window", prefWindow);
  //refBuilder->get_widget("emu_pref_window_two", prefWindowTwo);
  refBuilder->get_widget("emu_about_dialog", aboutDialog);
  
  //appMenu = refBuilder->get_object("app-menu");
  if(mainWindow)
  {
    
    prefWindow->set_type_hint(Gdk::WINDOW_TYPE_HINT_DIALOG);
    prefWindow->show();
    
    //prefWindowTwo->set_type_hint(Gdk::WINDOW_TYPE_HINT_DIALOG);
    //prefWindowTwo->show();
    
    const std::vector<Glib::ustring> authors = {
      "Colin Kinloch <colin@kinlo.ch>"
    };
    const std::vector<Glib::ustring> artists = {
    };
    
    aboutDialog->set_authors(authors);
   //aboutDialog->set_artists(artists);
    aboutDialog->set_copyright("© 2014 Colin Kinloch");
    aboutDialog->set_version(VERSION);
    
    aboutDialog->set_transient_for(*mainWindow);
    //aboutDialog->show();

    app->get_menu_by_id("app-menu");
    
    //app->set_app_menu(*appMenu);

    app->run(*mainWindow);
  }
  
  return 0;
}
