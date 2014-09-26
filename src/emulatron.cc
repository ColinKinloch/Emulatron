
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
#include "emu-header-b.h"
#include "emu-header-bar.hh"

Gtk::ApplicationWindow* mainWindow = nullptr;
Gtk::Window* prefWindow = nullptr;


int main(int argc, char **argv)
{
  Glib::RefPtr<Gtk::Application> app = Gtk::Application::create(argc, argv, "org.colinkinloch.emulator");
  
  Glib::RefPtr<Gtk::Builder> refBuilder = Gtk::Builder::create();

  //Gtk::Widget* header = Glib::wrap(emu_header_bar_new());

  Glib::RefPtr<Gtk::Settings> settings = Gtk::Settings::get_default();
  settings->property_gtk_application_prefer_dark_theme() = true;

  /*GtkWidget* header = g_object_new(EMU_TYPE_HEADER_BAR,
                                   "show-search-button", TRUE,
                                   "show-select-button", TRUE,
                                   "show-close-button", TRUE,
                                   "title", _("Video"),
                                   NULL);
*/

  //Gtk::Widget* header = Gtk::Widget(emu_header_bar_new());


  /*Gtk::Widget* header = Glib::Object(Glib::ConstructParams(Glib::Class(EMU_TYPE_HEADER_BAR),
                                     "show-search-button", TRUE,
                                     "show-select-button", TRUE,
                                     "show-close-button", TRUE,
                                     "title", _("Video"),
                                     static_cast<char*>(0)));*/
  //Glib::ConstructParams()

  Gtk::Widget* header = new Emu::HeaderBar();

  try
  {
    //gtk_widget_class_set_template_from_resource(, "./res/EmuMainToolbar.ui");
    //refBuilder->add_from_file("./res/EmuMainToolbar.ui");
    refBuilder->add_from_resource("/org/colinkinloch/emulatron/emulatron.ui");
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
  if(mainWindow && prefWindow)
  {
    //Get the GtkBuilder-instantiated Button, and connect a signal handler:
    
    //emuWindow->set_titlebar(*header);
      app->get_menu_by_id("appmenu");

    app->run(*mainWindow);
  }
  
  return 0;
}
