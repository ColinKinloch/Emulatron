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





static int latency = 20000; // start latency in micro seconds
static int sampleoffs = 0;
static short sampledata[300000];
static pa_buffer_attr bufattr;
static int underflows = 0;
static pa_sample_spec ss;

// This callback gets called when our context changes state.  We really only
// care about when it's ready or if it has failed
void pa_state_cb(pa_context *c, void *userdata) {
  pa_context_state_t state;
  int *pa_ready = (int *)userdata;
  state = pa_context_get_state(c);
  switch  (state) {
    // These are just here for reference
  case PA_CONTEXT_UNCONNECTED:
  case PA_CONTEXT_CONNECTING:
  case PA_CONTEXT_AUTHORIZING:
  case PA_CONTEXT_SETTING_NAME:
  default:
    break;
  case PA_CONTEXT_FAILED:
  case PA_CONTEXT_TERMINATED:
    *pa_ready = 2;
    break;
  case PA_CONTEXT_READY:
    *pa_ready = 1;
    break;
  }
}

static void stream_request_cb(pa_stream *s, size_t length, void *userdata) {
  pa_usec_t usec;
  int neg;
  pa_stream_get_latency(s,&usec,&neg);
  printf("  latency %8d us\n",(int)usec);
  if (sampleoffs*2 + length > sizeof(sampledata)) {
    sampleoffs = 0;
  }
  if (length > sizeof(sampledata)) {
    length = sizeof(sampledata);
  }
  pa_stream_write(s, &sampledata[sampleoffs], length, NULL, 0LL, PA_SEEK_RELATIVE);
  sampleoffs += length/2;
}

static void stream_underflow_cb(pa_stream *s, void *userdata) {
  // We increase the latency by 50% if we get 6 underflows and latency is under 2s
  // This is very useful for over the network playback that can't handle low latencies
  printf("underflow\n");
  underflows++;
  if (underflows >= 6 && latency < 2000000) {
    latency = (latency*3)/2;
    bufattr.maxlength = pa_usec_to_bytes(latency,&ss);
    bufattr.tlength = pa_usec_to_bytes(latency,&ss);
    pa_stream_set_buffer_attr(s, &bufattr, NULL, NULL);
    underflows = 0;
    printf("latency increased to %d\n", latency);
  }
}




Emulatron::Emulatron(int& argc, char**& argv):
  Gtk::Application(argc, argv, "org.colinkinloch.emulatron", Gio::APPLICATION_FLAGS_NONE)
{
  int r;
  int pa_ready = 0;
  int retval = 0;
  unsigned int a;
  double amp;

  for (a=0; a<sizeof(sampledata)/2; a++) {
    amp = cos(5000*(double)a/44100.0);
    sampledata[a] = amp * 32000.0;
  }

  pa_proplist *ctxProp = pa_proplist_new();
  pa_proplist_sets(ctxProp, PA_PROP_APPLICATION_ICON_NAME, "input-gaming");


  pa_ml = pa_mainloop_new();
  pa_mlapi = pa_mainloop_get_api(pa_ml);
  pa_ctx = pa_context_new_with_proplist(pa_mlapi, "Emulatron", ctxProp);
  pa_context_flags_t ctx_flags;
  pa_context_connect(pa_ctx, nullptr, (pa_context_flags_t)0, nullptr);

  pa_context_set_state_callback(pa_ctx, pa_state_cb, &pa_ready);

  while (pa_ready == 0) {
    pa_mainloop_iterate(pa_ml, 1, NULL);
  }

  ss.rate = 44100;
  ss.channels = 1;
  ss.format = PA_SAMPLE_S16LE;
  playstream = pa_stream_new(pa_ctx, "Playback", &ss, NULL);
  if (!playstream) {
    printf("pa_stream_new failed\n");
  }
  pa_stream_set_write_callback(playstream, stream_request_cb, NULL);
  pa_stream_set_underflow_callback(playstream, stream_underflow_cb, NULL);

  bufattr.fragsize = (uint32_t)-1;
  bufattr.maxlength = pa_usec_to_bytes(latency,&ss);
  bufattr.minreq = pa_usec_to_bytes(0,&ss);
  bufattr.prebuf = (uint32_t)-1;
  bufattr.tlength = pa_usec_to_bytes(latency,&ss);
  r = pa_stream_connect_playback(playstream, NULL, &bufattr, (pa_stream_flags_t)(PA_STREAM_INTERPOLATE_TIMING | PA_STREAM_ADJUST_LATENCY | PA_STREAM_AUTO_TIMING_UPDATE), NULL, NULL);

  if (r < 0) {
    // Old pulse audio servers don't like the ADJUST_LATENCY flag, so retry without that
    r = pa_stream_connect_playback(playstream, NULL, &bufattr,
                                   (pa_stream_flags_t)(PA_STREAM_INTERPOLATE_TIMING|
                                   PA_STREAM_AUTO_TIMING_UPDATE), NULL, NULL);
  }
  if (r < 0) {
    printf("pa_stream_connect_playback failed\n");
    retval = -1;
  }


  pa_mainloop_iterate(pa_ml, 1, NULL);

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
