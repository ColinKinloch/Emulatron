#include "config.h"

#include <random>
#include <iostream>

#include <gtkmm.h>
#include <gdkmm/pixbuf.h>

#include <SDL.h>
#include <GL/gl.h>

#include "emulatron.hh"
#include "emu-window.hh"

#include "libretro-core.hh"
#include "openvgdb.hh"

#include "emu-resources.h"

std::default_random_engine generator;
std::uniform_real_distribution<float> distribution(0,1);
auto rng = std::bind(distribution, generator);

static gboolean
render (GtkGLArea *area, GdkGLContext *context)
{
  glClearColor (rng(), rng(), rng(), 0);
  glClear (GL_COLOR_BUFFER_BIT);
  return true;
};

static void
resize (GtkGLArea *area, int width, int height)
{
  gtk_gl_area_make_current(area);
  glViewport(0,0,width,height);
};


Emulatron::Emulatron(int& argc, char**& argv):
  Gtk::Application(argc, argv, "org.colinkinloch.emulatron", Gio::APPLICATION_FLAGS_NONE)
{
  ao_initialize();
  defaultDriver = ao_default_driver_id();

  format.bits = 16;
  format.channels = 2;
  format.rate = 44100;
  format.byte_format = AO_FMT_LITTLE;

  audioDev = ao_open_live(defaultDriver, &format, NULL);
  if(audioDev == nullptr)
  {
    std::cerr<<"audio device fail"<<std::endl;
  }
  int buf_size = format.bits/8 * format.channels * format.rate;
  char* buffer = new char [buf_size];

  int sample;

	float freq = 440.0;
  for(int i=0; i<format.rate; i++){
    sample = (int)(0.75 * 32768.0 *
			sin(2 * M_PI * freq * ((float) i/format.rate)));

		/* Put the same stuff in left and right channel */
		buffer[4*i] = buffer[4*i+2] = sample & 0xff;
		buffer[4*i+1] = buffer[4*i+3] = (sample >> 8) & 0xff;
  }

  ao_play(audioDev, buffer, buf_size);

  Glib::init();
  Gio::init();
  
  Glib::RefPtr<Gio::File> openVGDBFile = Gio::File::create_for_path("./openvgdb.sqlite");
  OpenVGDB openVGDB = OpenVGDB(openVGDBFile);
  if(!openVGDBFile->query_exists())
  {
    std::cout<<"Downloading OpenVGDB"<<std::endl;
    //TODO Github releases api and glib-json
    //Glib::RefPtr<Gio::File> openVGDBUrl = Gio::File::create_for_uri("https://github.com/OpenVGDB/OpenVGDB/releases/latest");
    Glib::RefPtr<Gio::File> openVGDBUrl = Gio::File::create_for_uri("https://github.com/OpenVGDB/OpenVGDB/releases/download/v21.0/openvgdb.zip");
    //TODO Deal with zip
    //openVGDBFile->create_file()->splice(openVGDBUrl->read());
  }
  
  LibRetroCore dinothwar("./src/libretro-cores/Dinothawr/dinothawr_libretro.so");
  LibRetroCore bsnes("./src/libretro-cores/bsnes-libretro/out/bsnes_accuracy_libretro.so");
  LibRetroCore vbaNext("./src/libretro-cores/vba-next/vba_next_libretro.so");

  core = &vbaNext;

  core->init();
  core->loadGame(Gio::File::create_for_path("./fz.gba"));
  core->run();
  //core.reset();
  //core->deinit();

  if(SDL_Init(SDL_INIT_HAPTIC|SDL_INIT_GAMECONTROLLER) != 0)
  {
    std::cerr<<SDL_GetError()<<std::endl;
  }
  
  refBuilder = Gtk::Builder::create();
  refBuilder->set_application(Glib::RefPtr<Emulatron>(this));
  
  settings = Gtk::Settings::get_default();
  settings->property_gtk_application_prefer_dark_theme() = true;
  
  Glib::RefPtr<Gdk::Screen> screen = Gdk::Screen::get_default();
  Glib::RefPtr<Gtk::CssProvider> css = Gtk::CssProvider::create();
  
  css->load_from_file(Gio::File::create_for_uri("resource:///org/colinkinloch/emulatron/ui/emulatron.css"));
  
  Glib::RefPtr<Gtk::StyleContext> styleContext = Gtk::StyleContext::create();
  
  styleContext->add_provider_for_screen(screen,css,GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
  
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
    refBuilder->get_widget_derived("emu_main_window", emuWindow);
    refBuilder->get_widget_derived("emu_pref_window", prefWindow);
    refBuilder->get_widget_derived("emu_about_dialog", aboutDialog);
    refBuilder->get_widget("game_area", gameArea);
    refBuilder->get_widget("emu_main_stack", emuMainStack);
    //add_window(*emuWindow);
  }
  catch(const Gtk::BuilderError& ex)
  {
    std::cerr << "BuilderError: " << ex.what() << std::endl;
  }
  catch(std::runtime_error &ex)
  {
    std::cerr << "runtime_error: " << ex.what() << std::endl;
  }
  
  GtkWidget* glAreaWidget = gameArea->gobj();
  GtkGLArea* glArea = GTK_GL_AREA(glAreaWidget);
  gtk_gl_area_make_current(glArea);

  glClearColor(0,1,0,0);
  glClear(GL_COLOR_BUFFER_BIT);

  g_signal_connect(glArea, "render", G_CALLBACK(render), NULL);
  g_signal_connect(glArea, "resize", G_CALLBACK(resize), NULL);

  prefWindow->set_transient_for(*emuWindow);
  aboutDialog->set_transient_for(*emuWindow);
  
  add_action("view-gl",
    sigc::mem_fun(this, &Emulatron::view_gl));

  add_action("fullscreen",
    sigc::mem_fun(emuWindow, &EmuWindow::fullscreen));

  add_action("open",
    sigc::mem_fun(this, &Emulatron::on_open));
  
  add_action("quit",
    sigc::mem_fun(this, &Emulatron::on_quit));
  
  add_action("preferences",
    sigc::mem_fun(this, &Emulatron::on_preferences));
  add_action("about",
    sigc::mem_fun(this, &Emulatron::on_about));
  
  emuWindow->signal_hide().connect(sigc::mem_fun(this, &Emulatron::on_quit));
  
  register_application();
  
  add_window(*emuWindow);
  add_window(*prefWindow);
  add_window(*aboutDialog);
  
  emuWindow->show();
  
  auto actions = list_actions();
  for(auto it = actions.begin(); it != actions.end() ;++it)
   std::cout<<*it<<std::endl;
}

void Emulatron::view_gl()
{
  std::string child = "";
  if(emuMainStack->get_visible_child_name() == "game-selector") {
    emuMainStack->set_visible_child("game-view", Gtk::STACK_TRANSITION_TYPE_OVER_LEFT);
  }
  else {
    emuMainStack->set_visible_child("game-selector", Gtk::STACK_TRANSITION_TYPE_UNDER_RIGHT);
  }
}

void Emulatron::on_open()
{
  std::cout<<"Open rom!"<<std::endl;
}

void Emulatron::on_preferences()
{
  prefWindow->show();
}

void Emulatron::on_about()
{
  aboutDialog->show();
}
void Emulatron::on_quit()
{
  std::cout << "Goodbye!" << std::endl;
  quit();
}

int main(int argc, char **argv)
{
  Glib::RefPtr<Emulatron> app = Glib::RefPtr<Emulatron>(new Emulatron(argc, argv));
  
  app->run(argc, argv);
  
  return 0;
}
